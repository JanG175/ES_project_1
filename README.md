To add the BRAM driver to the project, use:
```bash
git submodule update --init --recursive
```

Install packages to show iamges in the terminal:
```bash
echo 'deb [trusted=yes] https://apt.fury.io/ascii-image-converter/ /' | sudo tee /etc/apt/sources.list.d/ascii-image-converter.list
sudo apt update
sudo apt install -y ascii-image-converter
```