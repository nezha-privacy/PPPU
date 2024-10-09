#pragma once

#include <memory>
#include <string>
#include <vector>
#include <fstream>
#include <random>
#include <cstdlib>
#include <math.h>
#include <type_traits>
#include <random>

#include "tutorial_1_prepare.cpp"
#include "tutorial_2_context.cpp"
#include "DataStructure/Smap.cpp"
namespace tutorial {
void build_smap1(int pid,int num_parties){
    // Prepare context
    auto context = run_player(pid, num_parties);
    ctx = context.get();
    std::vector<float> input1 = {{10.3,12.6,7.9,5.1,1.3}};
    std::vector<float> input2 = {{6.2,12.3,6.7,9.5,10.2}};

    // Prepare data
    // Make Value
    auto value1 = make_value<float, Value128>(ctx, pid, input1, pppu::Visibility::Public());
    auto value2 = make_value<float, Value128>(ctx, pid, input2, pppu::Visibility::Public());
    
    //construct function1
    pppu::Smap<Value128,Value128> smap(ctx,value1,value2);

    //get value by key
    std::cout<<smap.find(12.6).to_string()<<'\n';

    //insert key-value pair
    smap.insert(19.5,12.3);
    std::cout<<smap.find(19.5).to_string()<<'\n';

    //get key-value pair by idx
    auto pr=smap[5];
    std::cout<<pr.first.to_string()<<" "<<pr.second.to_string()<<'\n';

    //sort by key or value
    smap.sort_by_key();
    pr=smap[0];
    std::cout<<pr.first.to_string()<<" "<<pr.second.to_string()<<'\n';

    smap.sort_by_value();
    pr=smap[0];
    std::cout<<pr.first.to_string()<<" "<<pr.second.to_string()<<'\n';

    //change value by key
    smap.change(12.6,10.9);
    std::cout<<smap.find(12.6).to_string()<<'\n';

    //change visibility
    auto value3 = make_value<float, Value128>(ctx, pid, input1, pppu::Visibility::Public());
    auto value4 = make_value<float, Value128>(ctx, pid, input2, pppu::Visibility::Private(0));
    pppu::Smap<Value128,Value128> smap1(ctx,value3,value4);
    std::cout<<smap1.find(12.6).to_string()<<'\n';
    smap1.change_Visibility(pppu::Visibility::Public(),pppu::Visibility::Public());
    std::cout<<smap1.find(12.6).to_string()<<'\n';

    auto value5 = make_value<float, Value128>(ctx, pid, input1, pppu::Visibility::Share());
    auto value6 = make_value<float, Value128>(ctx, pid, input2, pppu::Visibility::Share());
    pppu::Smap<Value128,Value128> smap2(ctx,value5,value6);
    if(pid==0) smap2.insert(12.5,0);
    else if(pid==1) smap2.insert(0,6.8);
    else smap2.insert(0,0);
    smap2.change_Visibility(pppu::Visibility::Public(),pppu::Visibility::Public());
    std::cout<<smap2.find(12.5).to_string()<<'\n';

}
void build_smap2(int pid, int num_parties){
     // Prepare context
    auto context = run_player(pid, num_parties);
    ctx = context.get();
    pppu::Smap<Value128,Value128> smap(ctx);
    //set visibility
    smap.set_Visibility(pppu::Visibility::Public(),pppu::Visibility::Public());

    //defaule 0
    smap.set_fracbits(10,10);

    //insert data
    smap.insert(10.3,6.2);
    smap.insert(12.6,12.3);
    smap.insert(7.9,6.7);
    smap.insert(5.1,9.5);
    smap.insert(1.3,1.1);

    //get value by key
    std::cout<<smap.find(12.6).to_string()<<'\n';

    //insert key-value pair
    smap.insert(19.5,12.3);
    std::cout<<smap.find(19.5).to_string()<<'\n';

    //get key-value pair by idx
    auto pr=smap[5];
    std::cout<<pr.first.to_string()<<" "<<pr.second.to_string()<<'\n';

    //sort by key or value
    smap.sort_by_key();
    pr=smap[0];
    std::cout<<pr.first.to_string()<<" "<<pr.second.to_string()<<'\n';

    smap.sort_by_value();
    pr=smap[0];
    std::cout<<pr.first.to_string()<<" "<<pr.second.to_string()<<'\n';

    //change value by key
    smap.change(12.6,10.9);
    std::cout<<smap.find(12.6).to_string()<<'\n';
}
void smap_test1(int pid,int num_parties){

    std::random_device rd;  
    std::mt19937 gen(rd()); 
    std::uniform_real_distribution<double> dis(-1e18, 1e18);
    auto context = run_player(pid, num_parties);
    ctx = context.get();

    pppu::Smap<Value128,Value128> smap(ctx);

    //set visibility
    smap.set_Visibility(pppu::Visibility::Public(),pppu::Visibility::Public());

    //defaule 0
    smap.set_fracbits(40,40);

    //cost time 136.164 ms
    for(int64_t i=0;i<1000000;i++) smap.insert(dis(gen),dis(gen));


    std::vector<double> input1,input2;
    for(int i=0;i<1000000;i++) input1.push_back(dis(gen)),input2.push_back(dis(gen));

    auto value1 = make_value<double, Value128>(ctx, pid, input1, pppu::Visibility::Public());
    auto value2 = make_value<double, Value128>(ctx, pid, input2, pppu::Visibility::Public());
    
    //cost time 24.7002ms
    pppu::Smap<Value128,Value128> smap1(ctx,value1,value2);

}

void smap_test2(int pid, int num_parties){
    std::random_device rd;  
    std::mt19937 gen(rd()); 
    std::uniform_real_distribution<double> dis(-1e18, 1e18);

    auto context = run_player(pid, num_parties);
    ctx = context.get();
    std::vector<double> input1,input2;
    for(int i=0;i<1000000;i++) {  
        input1.push_back(dis(gen)),input2.push_back(dis(gen));
    }
    auto value1 = make_value<double, Value128>(ctx, pid, input1, pppu::Visibility::Share());
    auto value2 = make_value<double, Value128>(ctx, pid, input2, pppu::Visibility::Share());

    pppu::Smap<Value128,Value128> smap(ctx,value1,value2);

    //z128 1e6 size data cost time 1708 s
    //z64 1e6 size data cost time 724 s
    //z128 1e5 size data cost time 125075 ms
    smap.sort_by_key();

}
void smap_test3(int pid,int num_parties){
    std::random_device rd;  
    std::mt19937 gen(rd()); 
    std::uniform_real_distribution<double> dis(-1e18, 1e18);

    auto context = run_player(pid, num_parties);
    ctx = context.get();
    std::vector<double> input1,input2;
    for(int i=0;i<1000000;i++) {  
        input1.push_back(dis(gen)),input2.push_back(dis(gen));
    }
    auto value1 = make_value<double, Value128>(ctx, pid, input1, pppu::Visibility::Share());
    auto value2 = make_value<double, Value128>(ctx, pid, input2, pppu::Visibility::Share());

    pppu::Smap<Value128,Value128> smap(ctx,value1,value2);

    //cost time 1.5s
    smap.change_Visibility(pppu::Visibility::Public(),pppu::Visibility::Public());

    
}
void tutorial_7_smap(int pid, int num_parties) {
    

    build_smap1(pid,num_parties);
    // build_smap2(pid,num_parties);
    // smap_test1(pid,num_parties);
    // smap_test2(pid,num_parties);
    // smap_test3(pid,num_parties);
    
}

}
