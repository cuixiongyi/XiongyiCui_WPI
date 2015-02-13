#compile Vim from source https://github.com/Valloric/YouCompleteMe/wiki/Building-Vim-from-source
sudo apt-get install libncurses5-dev libgnome2-dev libgnomeui-dev \
    libgtk2.0-dev libatk1.0-dev libbonoboui2-dev \
    libcairo2-dev libx11-dev libxpm-dev libxt-dev python-dev \
    ruby-dev mercurial
#Remove vim
sudo apt-get remove vim-tiny vim-common vim-gui-common
sudo apt-get remove vim vim-runtime gvim
sudo apt-get install checkinstall
cd ~
hg clone https://code.google.com/p/vim/
cd vim
./configure --with-features=huge \
            --enable-multibyte \
            --enable-rubyinterp \
            --enable-pythoninterp \
            --with-python-config-dir=/usr/lib/python2.7/config \
            --enable-perlinterp \
            --enable-luainterp \
            --enable-gui=gtk2 --enable-cscope --prefix=/usr
make VIMRUNTIMEDIR=/usr/share/vim/vim74
sudo checkinstall make install

sudo update-alternatives --install /usr/bin/editor editor /usr/bin/vim 1
sudo update-alternatives --set editor /usr/bin/vim
sudo update-alternatives --install /usr/bin/vi vi /usr/bin/vim 1
sudo update-alternatives --set vi /usr/bin/vim

git clone git://github.com/amix/vimrc.git ~/.vim_runtime
sh ~/.vim_runtime/install_awesome_vimrc.sh

#down global from http://www.gnu.org/software/global/download.html
echo    http://tamacom.com/global/global-6.3.4.tar.gz
cd ~/downloads
tar -xvf global-6.3.4.tar.gz 
cd global-6.3.4/
sudo apt-get install autoconf bison flex gperf
sh reconf.sh 
./configure
sudo checkinstall
#build gnu global

git clone https://github.com/gmarik/Vundle.vim.git ~/.vim/bundle/Vundle.vim
cp ./.vimrc ~/
mkdir ~/.vim/
cp -R ./plugin/ ~/.vim/
vi +PluginInstall +qall
