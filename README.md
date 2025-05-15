# Installing necessary packages inside WSL:

## FFT and python essentials
sudo apt update && sudo apt install -y \
build-essential cmake pkg-config \
libfftw3-dev \
python3 python3-dev python3-pip

## Python packages for plotting
python3 -m pip install --user numpy matplotlib