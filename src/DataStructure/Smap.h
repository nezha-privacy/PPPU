#pragma once

#include<utility>
#include<map>
#include <stdexcept>
#include<variant>

#include<context/value.hpp>
#include<context/visibility.h>
#include<context/shape/sort.hpp>
namespace pppu{

template<typename kValue,typename vValue>
class Smap{
public:
    using kPlainType=kValue::PlainType::value_type;
    using kShareType=kValue::ShareType::value_type;
    using vPlainType=vValue::PlainType::value_type;
    using vShareType=vValue::ShareType::value_type;
    vPlainType zero = vPlainType(0);
protected:
    bool is_empty=true;
    bool key_is_plain=false;
    bool value_is_plain=false;
    Visibility vis_key,vis_val;
    kValue key;
    vValue value;
    std::map<kPlainType,std::variant<vPlainType,vShareType>> mp;
    int64_t key_fracbits=0;
    int64_t value_fracbits=0;
    Context *ctx;
public:
    Smap()                       = default;
    ~Smap()                      = default;
    Smap(Smap&&)                 = default;
    Smap(Smap const&)            = default;
    Smap& operator=(Smap&&)      = default;
    Smap& operator=(Smap const&) = default;

    Smap(Context *ctx):ctx(ctx){};

    Smap(Context *ctx,kValue &key,vValue &value):ctx(ctx),key(key),value(value){
        if(key.ndim()>1||value.ndim()>1){
            throw std::invalid_argument("key or value's dimension is not 1");
        }
        if(key.numel()!=value.numel()){
            throw std::invalid_argument("key or value's numel not equal");
        }
        if(key.visibility().is_invalid()||value.visibility().is_invalid()){
            throw std::invalid_argument("invalid visibility");
        }
        this->vis_key=key.visibility();
        this->vis_val=value.visibility();
        this->is_empty=false;

        this->set_fracbits(key.fracbits(),value.fracbits());
        if(!this->vis_key.is_share()) this->key_is_plain=true;
        if(!this->vis_val.is_share()) this->value_is_plain=true;
        this->store();
    }

    int64_t numel(){return this->key.numel();}

    void store();

    Visibility const& get_key_visibility(){
        return this->vis_key;
    }

    Visibility const& get_value_visibility(){
        return this->vis_val;
    }
    kValue const& get_key(){
        return this->key;
    }
    vValue const& get_value(){
        return this->valuel;
    }

    void set_Visibility(Visibility vis1,Visibility vis2);

    void change_Visibility(Visibility vis1,Visibility vis2);

    void set_fracbits(int64_t f1,int64_t f2){
        this->key_fracbits=f1;
        this->value_fracbits=f2;
    }

    template<typename kType,typename vType,typename T,typename K>
    void DoInsert(T k,K v);

    template<typename T,typename K>
    void insert(T k,K v);

    template<typename kType,typename vType,typename T>
    vType const& Dofind(T x,int8_t player_id);

    template<typename T>
    auto const& find(T x);

    template<typename kType,typename vType,typename T,typename K>
    void Dochange(T k,K v,int8_t player_id);

    template<typename T,typename K>
    void change(T k,K v);


    void sort_by_key();

    
    void sort_by_value();

    template<typename kType,typename vType>
    std::pair<kType ,vType> Doat(int64_t pos){
        auto& key_data=this->key_is_plain?this->key.data_p():this->key.data_s();
        auto& value_data=this->value_is_plain?this->value.data_p():this->value.data_s();
        return std::make_pair(key_data.data()[pos],value_data.data()[pos]);
    }

    auto at(int64_t pos){
        if(this->key_is_plain&&this->value_is_plain) return this->Doat<kPlainType,vPlainType>(pos);
        if(!this->key_is_plain&&this->value_is_plain) return this->Doat<kShareType,vPlainType>(pos);
        if(this->key_is_plain&&!this->value_is_plain) return this->Doat<kPlainType,vShareType>(pos);
        if(!this->key_is_plain&&!this->value_is_plain) return this->Doat<kShareType,vShareType>(pos);
    }

    auto operator[](int64_t pos){
        if(this->key_is_plain&&this->value_is_plain) return this->Doat<kPlainType,vPlainType>(pos);
        if(!this->key_is_plain&&this->value_is_plain) return this->Doat<kShareType,vPlainType>(pos);
        if(this->key_is_plain&&!this->value_is_plain) return this->Doat<kPlainType,vShareType>(pos);
        if(!this->key_is_plain&&!this->value_is_plain) return this->Doat<kShareType,vShareType>(pos);
    }

    void open_key();

    void open_value();
};  
}
