%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
Compiz Keystone Plugin
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% Install Dependencies
sudo apt-get install compiz-dev compizconfig-settings-manager

% Setup build directory (you might need to adjust install dir to /usr)
mkdir build
cd build
cmake ..

% build and install
make
sudo make install

% Launch compiz settings
ccsm
