if ! command -v cmake
then
    echo "Please install cmake"
    exit 1
fi
cmake -Bbuild
cd build
make 
sudo mv conch /usr/bin/conch
