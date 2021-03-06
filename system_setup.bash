sudo apt-get install -y git
git config --global user.name "Xiongyi Cui"
git config --global user.email cuixiongyi2013@gmail.com
git config --global credential.helper "cache --timeout=7200"
cp ./configFile/.gitignore_global ~/
git config --global core.excludesfile ~/.gitignore_global
 
sudo apt-get install -y subversion
sudo apt-get install -y build-essential
sudo apt-get install -y terminator
sudo apt-get install -y vim
sudo apt-get install -y python-wstool
sudo apt-get install -y meld 


# for jeklly
sudo apt-get install nodejs
sudo gem install jekyll
sudo gem install bundler
bundle install
# processes manegmant 
sudo apt-get install -y htop

apt-get build-dep -y emacs24
sudo apt-get install -y emacs24



mkdir ~/repo/
cd ~/repo/
git clone https://github.com/cuixiongyi/XiongyiCui_WPI
git clone https://github.com/cuixiongyi/cxy_leetcode

# install Vundle
git clone https://github.com/gmarik/Vundle.vim.git ~/.vim/bundle/Vundle.vim
cp -R ~/repo/XiongyiCui_WPI/Vim/plugin/ ~/.vim/
cp -R ~/repo/XiongyiCui_WPI/Vim/.vimrc ~/
vim +PluginInstall +qall

# install YCM
# disable YCM for now
#cd ~/.vim/bundle
#git clone https://github.com/Valloric/YouCompleteMe.git
#cd YouCompleteMe
#git submodule update --init --recursive
#./install.sh --clang-completer  --system-libclang

# install ROS
sudo sh -c 'echo "deb http://packages.ros.org/ros/ubuntu $(lsb_release -sc) main" > /etc/apt/sources.list.d/ros-latest.list'

sudo apt-key adv --keyserver hkp://pool.sks-keyservers.net --recv-key 0xB01FA116
sudo apt-get update
sudo apt-get install -y ros-jade-desktop-full
sudo rosdep init
rosdep update
source /opt/ros/jade/setup.bash
# install catkin workspace
sudo apt-get install -y cmake python-catkin-pkg python-empy python-nose python-setuptools libgtest-dev build-essential
sudo apt-get install -y ros-jade-catkin



#!/bin/bash
  onlyroot="Do not run this as root."
  
if [ $(whoami) == 'root' ];then
echo -e $COLOR$onlyroot$MONO #"Only root can do this operation."
    #DBG_MSG "exit 5"
    exit 0
fi



sudo rosdep init
rosdep update
source /opt/ros/jade/setup.bash
sudo apt-get install -y python-rosinstall
mkdir -p ~/cxy_workspace/src
cd ~/cxy_workspace/src
catkin_init_workspace
cd ~/cxy_workspace/
catkin_make
source devel/setup.bash

echo '########## system_setup.bash  START' >> ~/.bashrc
echo source /opt/ros/jade/setup.bash >> ~/.bashrc
#echo source /usr/share/drcsim/setup.sh >> ~/.bashrc
echo source ~/cxy_workspace/devel/setup.bash >> ~/.bashrc
#echo export ATLAS_ROBOT_INTERFACE=~/cxy_workspace/src/drc/bdi_api/AtlasRobotInterface >> ~/.bashrc
#echo export LD_LIBRARY_PATH='${LD_LIBRARY_PATH}':'${ATLAS_ROBOT_INTERFACE}'/lib64 >> ~/.bashrc
echo alias yhome="'cd ~/cxy_workspace/src/cxyros'" >> ~/.bashrc
echo alias ymake="'catkin_make install -DCMAKE_INSTALL_PREFIX:PATH=~/cxy_workspace/install -C ~/cxy_workspace -DCMAKE_BUILD_TYPE=Release'" >> ~/.bashrc
echo export ROS_PACKAGE_PATH=~/cxy_workspace/src/cxyros:/opt/ros/jade/share >> ~/.bashrc
echo alias repohome="'cd ~/repo/'" >> ~/.bashrc
echo alias ypage="'bundle exec jekyll serve'" >> ~/.bashrc
echo alias leet-make="'cd ~/repo/build/cxy_leetcode/ && cmake ~/repo/cxy_leetcode/ && make install'" >> ~/.bashrc
echo '########## system_setup.bash  END' >> ~/.bashrc
#echo alias drceclipse="'catkin_make --force-cmake -G\"Eclipse CDT4 - Unix Makefiles\" -DCMAKE_BUILD_TYPE=Debug -DCMAKE_ECLIPSE_MAKE_ARGUMENTS=-j8 -C ~/cxy_workspace'" >> ~/.bashrc
#echo export GAZEBO_PLUGIN_PATH=~/cxy_workspace/devel/lib:'${GAZEBO_PLUGIN_PATH}' >> ~/.bashrc
#echo export GAZEBO_MODEL_PATH=~/cxy_workspace/src/drc/field/robotiq:'${GAZEBO_MODEL_PATH}' >> ~/.bashrc
#echo export PYTHONPATH=~/cxy_workspace/src/drc/trajopt/build_trajopt/lib:~/cxy_workspace/src/drc/trajopt:'${PYTHONPATH}' >> ~/.bashrc
#echo export LD_LIBRARY_PATH='${LD_LIBRARY_PATH}':/usr/lib >> ~/.bashrc
#echo export GUROBI_HOME="/opt/gurobi602/linux64" >> ~/.bashrc
#echo export OPENRAVE_DATA='${OPENRAVE_DATA}':~/cxy_workspace/src/drc/trajopt/ >> ~/.bashrc
#echo alias drctrajopt="'cd ~/cxy_workspace/src/drc/trajopt/'" >> ~/.bashrc
source ~/.bashrc
cd ~/cxy_workspace/src

# wstool is for workspace overlay 
# see http://wiki.ros.org/catkin/Tutorials/workspace_overlaying
wstool init
wstool set cxyros https://github.com/cuixiongyi/cxy-ros --git
source ~/.bashrc
wstool update cxyros
. ~/.bashrc
cd ~/cxy_workspace/
catkin_make
source ~/.bashrc
