#pragma once

#include "tutorial_1_prepare.cpp"
#include "tutorial_2_context.cpp"
#include "tutorial_3_mpc.cpp"
#include "tutorial_4_network.cpp"
#include "tutorial_5_ndarray.cpp"
#include "tutorial_6_visibility.cpp"

int main(int argc, char* argv[]) {
    int pid = std::stoi(argv[1]);
    int num_parties = std::stoi(argv[2]);

    /// @note Each time can only run one single tutorial
    /// @example The first terminal  : ./tutorial 0 2
    ///          The Second terminal : ./tutorial 1 2
    tutorial::tutorial_1_prepare(pid, num_parties);
    //tutorial::tutorial_2_context(pid, num_parties);
    //tutorial::tutorial_3_mpc(pid, num_parties);
    //tutorial::tutorial_4_network(pid, num_parties);
    //tutorial::tutorial_5_ndarray();
    //tutorial::tutorial_6_visibility(pid, num_parties);
    return 0;
}
