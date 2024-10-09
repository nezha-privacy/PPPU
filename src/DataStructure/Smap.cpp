#include "Smap.h"
namespace pppu
{
    template <typename kValue, typename vValue>
    void Smap<kValue, vValue>::store()
    {
        if (this->vis_key.is_public() || this->vis_key.is_private())
        {
            auto k = this->key.data_p();
            if (this->vis_val.is_public() || this->vis_val.is_private())
            {
                auto v = this->value.data_p();
                for (int i = 0; i < (k.numel()); i++)
                {   
                    this->mp[k.data()[i]].template emplace<0>(v.data()[i]);
                }
            }
            else
            {
                auto v = this->value.data_s();
                for (int i = 0; i < (k.numel()); i++)
                {
                    this->mp[k.data()[i]].template emplace<1>(v.data()[i]);
                }
            }
        }
    }
    template <typename kValue, typename vValue>
    void Smap<kValue, vValue>::set_Visibility(Visibility vis1, Visibility vis2)
    {
        if (vis1.is_invalid() || vis2.is_invalid())
        {
            throw std::invalid_argument("invalid Visibility");
        }
        if(!this->vis_key.is_invalid()||!this->vis_key.is_invalid()){
             throw std::invalid_argument("unable to set visibiilty");
        }
        this->vis_key = vis1;
        this->vis_val = vis2;
        this->key.set_visibility(vis1,1);
        this->value.set_visibility(vis2,1);
        if(this->vis_key.is_public()||this->vis_key.is_private()) this->key_is_plain=true;
        if(this->vis_val.is_public()||this->vis_val.is_private()) this->value_is_plain=true;
    }
    template <typename kValue, typename vValue>
    void Smap<kValue, vValue>::change_Visibility(Visibility vis1, Visibility vis2)
    {
        if (!this->vis_key.is_public())
        {
            if (vis1.is_public()) this->open_key();
            else if (vis1.is_share())
            {
                if (this->vis_key.is_private())
                {
                    this->key = pppu::input(this->ctx, this->key);
                }
                else
                    throw std::invalid_argument("cant change visibility");
            }
            else
                throw std::invalid_argument("cant change visibility");
            this->vis_key = vis1;
        }
        if (!this->vis_val.is_public())
        {
            if (vis2.is_public()) this->open_value();
            else if (vis2.is_share())
            {
                if (this->vis_val.is_private())
                {
                    this->value = pppu::input(this->ctx, this->value);
                }
                else
                    throw std::invalid_argument("cant change visibility");
            }
            else
                throw std::invalid_argument("cant change visibility");
            this->vis_val = vis2;
        }
        if(this->vis_key.is_public()||this->vis_key.is_private()) this->key_is_plain=true;
        if(this->vis_val.is_public()||this->vis_val.is_private()) this->value_is_plain=true;
        this->store();
    }

    template<typename kValue,typename vValue>
    template<typename kType,typename vType,typename T,typename K>
    void Smap<kValue,vValue>::DoInsert(T k,K v){
        auto key_current = static_cast<kType::value_type>(std::round(std::ldexp(k, this->key_fracbits)));
        auto value_current = static_cast<vType::value_type>(std::round(std::ldexp(v, this->value_fracbits)));
        if (this->is_empty)
        {
            this->is_empty=false;
            auto key_init = core::make_ndarray(std::vector<kType>({kType(key_current)}));
            auto value_init = core::make_ndarray(std::vector<vType>({vType(value_current)}));
            if (this->key_is_plain)
                this->key.assign_p(key_init);
            else
                this->key.assign_s(key_init);
            if (this->value_is_plain)
                this->value.assign_p(value_init);
            else
                this->value.assign_s(value_init);
            return;
        }

        auto &key_data = this->key_is_plain ? this->key.data_p() : this->key.data_s();
        auto &value_data = this->value_is_plain ? this->value.data_p() : this->value.data_s();

        key_data.push_back(kType(key_current));
        value_data.push_back(vType(value_current));

        if (this->key_is_plain){

            if(this->value_is_plain) this->mp[kType(key_current)].template emplace<0>(vType(value_current));
            else this->mp[kType(key_current)].template emplace<1>(vType(value_current));

        }
    }
    template <typename kValue, typename vValue>
    template <typename T, typename K>
    void Smap<kValue, vValue>::insert(T k, K v)
    {
        if(this->key_is_plain&&this->value_is_plain) this->DoInsert<kPlainType,vPlainType,T,K>(k,v);
        if(!this->key_is_plain&&this->value_is_plain) this->DoInsert<kShareType,vPlainType,T,K>(k,v);
        if(this->key_is_plain&&!this->value_is_plain) this->DoInsert<kPlainType,vShareType,T,K>(k,v);
        if(!this->key_is_plain&&!this->value_is_plain) this->DoInsert<kShareType,vShareType,T,K>(k,v);
    }

    template <typename kValue, typename vValue>
    template<typename kType,typename vType,typename T>
    vType const & Smap<kValue, vValue>::Dofind(T x, int8_t player_id)
    {
        auto x_current = kType(static_cast<kType::value_type>(std::round(std::ldexp(x, this->key_fracbits))));
        if (this->key_is_plain)
        {
            if (this->vis_key.is_public())
            {
                if (this->mp.find(x_current) != this->mp.end()){
                    if(this->vis_val.is_public()) return std::get<0>(mp[x_current]);
                    else if(this->vis_val.is_private()) {
                        if(this->vis_val.owner()==this->ctx->pid())  return std::get<0>(mp[x_current]);
                        else return zero;
                    }
                    else return std::get<1>(mp[x_current]);
                }
                else
                {
                    throw std::runtime_error("key not found");
                }
            }
            else
            {
                if (this->vis_key.owner() != player_id)
                {
                    throw std::runtime_error("key is private and you dont have the key");
                }
                else
                {
                    if (this->mp.find(x_current) != this->mp.end())
                    {
                        if(this->vis_val.is_public()) return std::get<0>(mp[x_current]);
                        else if(this->vis_val.is_private()) {
                            if(this->vis_val.owner()==this->ctx->pid())  return std::get<0>(mp[x_current]);
                            else return zero;
                        }
                        else return std::get<1>(mp[x_current]);
                    }
                    else
                    {
                        throw std::runtime_error("key not found");
                    }
                }
            }
        }
        else
        {
            throw std::runtime_error("key is shared now,please reconstruct to get the key");
        }
    }
    template <typename kValue, typename vValue>
    template <typename T>
    auto const & Smap<kValue, vValue>::find(T x)
    {
        int8_t player_id=this->ctx->pid();
        if(this->key_is_plain&&this->value_is_plain) return this->Dofind<kPlainType,vPlainType,T>(x,player_id);
        if(!this->key_is_plain&&this->value_is_plain) return this->Dofind<kShareType,vPlainType,T>(x,player_id);
        if(this->key_is_plain&&!this->value_is_plain) return this->Dofind<kPlainType,vShareType,T>(x,player_id);
        if(!this->key_is_plain&&!this->value_is_plain) return this->Dofind<kShareType,vShareType,T>(x,player_id);
    }

    template <typename kValue, typename vValue>
    template<typename kType,typename vType,typename T,typename K>
    void Smap<kValue, vValue>::Dochange(T k, K v, int8_t player_id )
    {
        auto k_current = kType(static_cast<kType::value_type>(std::round(std::ldexp(k, this->key_fracbits))));
        auto v_current = vType(static_cast<vType::value_type>(std::round(std::ldexp(v, this->value_fracbits))));
        if (this->key_is_plain)
        {
            if (this->vis_key.is_public())
            {
                if (this->mp.find(k_current) != this->mp.end())
                {
                    if (this->vis_val.is_public())
                        mp[k_current].template emplace<0>(v_current);
                    else if (this->vis_val.is_private())
                    {
                        if (player_id == this->vis_val.owner())
                            mp[k_current].template emplace<1>(v_current);
                        else
                            throw std::runtime_error("cant change the value due to visibility");
                    }
                    else
                        throw std::runtime_error("cant change the value due to visibility");
                }
                else
                {
                    throw std::runtime_error("key not found");
                }
            }
            else
            {
                if (this->vis_key.owner() != player_id)
                {
                    throw std::runtime_error("key is private and you dont have the key");
                }
                else
                {
                    if (this->mp.find(k_current) != this->mp.end())
                    {
                        if (this->vis_val.is_public())
                            mp[k_current].template emplace<0>(v_current);

                        else if (this->vis_val.is_private())
                        {
                            if (player_id == this->vis_val.owner())
                                mp[k_current].template emplace<1>(v_current);
                            else
                                throw std::runtime_error("cant change the value due to visibility");
                        }

                        else
                            throw std::runtime_error("cant change the value due to visibility");
                    }
                    else
                    {
                        throw std::runtime_error("key not found");
                    }
                }
            }
        }
        else
        {
            throw std::runtime_error("key is shared now,please reconstruct to get the key");
        }
    }
    template <typename kValue, typename vValue>
    template <typename T, typename K>
    void Smap<kValue, vValue>::change(T k, K v)
    {
        int8_t player_id=this->ctx->pid();
        if(this->key_is_plain&&this->value_is_plain) this->Dochange<kPlainType,vPlainType,T,K>(k,v,player_id);
        if(!this->key_is_plain&&this->value_is_plain) this->Dochange<kShareType,vPlainType,T,K>(k,v,player_id);
        if(this->key_is_plain&&!this->value_is_plain) this->Dochange<kPlainType,vShareType,T,K>(k,v,player_id);
        if(!this->key_is_plain&&!this->value_is_plain) this->Dochange<kShareType,vShareType,T,K>(k,v,player_id);
    }
    template <typename kValue, typename vValue>
    void Smap<kValue, vValue>::sort_by_key()
    {
        auto &key_data = this->key_is_plain ? this->key.data_p() : this->key.data_s();
        auto &value_data = this->value_is_plain ? this->value.data_p() : this->value.data_s();
        if (this->vis_key.is_public())
        {
            std::vector<int64_t> idx(this->key.numel());
            for (int64_t i = 0; i < idx.size(); ++i)
                idx[i] = i;
            std::sort(idx.begin(), idx.end(), [&key_data](int64_t i1, int64_t i2)
                      { return key_data.data()[i1] < key_data.data()[i2]; });
            key_data=key_data.permute(idx);
            value_data=value_data.permute(idx);
        }
        else if (this->vis_key.is_private())
        {
            throw std::runtime_error("key is private");
        }
        else
        {
            pppu::sort(this->ctx, this->key, this->value);
        }
    }

    template <typename kValue, typename vValue>
    void Smap<kValue, vValue>::sort_by_value()
    {
        auto &key_data = this->key_is_plain ? this->key.data_p() : this->key.data_s();
        auto &value_data = this->value_is_plain ? this->value.data_p() : this->value.data_s();
        if (this->vis_val.is_public())
        {
            std::vector<int64_t> idx(this->value.numel());
            for (int64_t i = 0; i < idx.size(); ++i)
                idx[i] = i;
            std::sort(idx.begin(), idx.end(), [&value_data](int64_t i1, int64_t i2)
                      { return value_data.data()[i1] < value_data.data()[i2] ;});
            key_data=key_data.permute(idx);
            value_data=value_data.permute(idx);
        }
        else if (this->vis_val.is_private())
        {
            throw std::runtime_error("key is private");
        }
        else
        {
            pppu::sort(this->ctx, this->value, this->key);
        }
    }
    template <typename kValue, typename vValue>
    void Smap<kValue, vValue>::open_key()
    {
        if (this->vis_key.is_share())
        {
            this->key = open(this->ctx, this->key);
        }
        else if (this->vis_key.is_private())
        {
            auto tmp = pppu::input(this->ctx, this->key);
            this->key = open(this->ctx, tmp);
        }
    }
    template <typename kValue, typename vValue>
    void Smap<kValue, vValue>::open_value()
    {
        if (this->vis_val.is_share())
        {
            this->value = open(this->ctx, this->value);
        }
        else if (this->vis_val.is_private())
        {
            auto tmp = pppu::input(this->ctx, this->value);
            this->value = open(this->ctx, tmp);
        }
    }
}
