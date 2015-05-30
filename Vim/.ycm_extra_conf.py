flags = [
        '-Wall',
        '-Wextra',
        '-std=c++11',
        'c++',

        #Standard includes
        '-isystem',
        '/usr/include/c++/4.7',

        #current dir
        '-I',
        '.',

        #custom libraries
        '-I',
        '/opt/boost_1_55_0',
        '-I',
        '/opt/cryptopp-5.6.2',
        '-I',
        '/opt/llvm_install/include',
        '-I',
        '/opt/ros/indigo/include/opencv2/',

        '-I',
        '/usr/include/eigen3/Eigen',

        '-I',
        '/opt/ros/indigo/include/pcl_ros/'
        ]
