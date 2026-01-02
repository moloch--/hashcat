# test.sh usage

Hashcat's unit tests: full background in [docs/hashcat-plugin-development-guide.md](docs/hashcat-plugin-development-guide.md).
Small summary on how to use:

### Install pre-requisites
```
# Allow local installation of perl modules (such that we don't need root)
#  https://gwcbi.github.io/HPC/perl.html
cd $HOME
mkdir .perl
wget -O- http://cpanmin.us | perl - -l $HOME/.perl5 App::cpanminus local::lib
eval $(perl -I $HOME/.perl5/lib/perl5 -Mlocal::lib=$HOME/.perl5)
echo 'eval $(perl -I $HOME/.perl5/lib/perl5 -Mlocal::lib=$HOME/.perl5)' >> .bashrc
echo 'export MANPATH=$HOME/.perl5/man:$MANPATH' >> .bashrc

# Install pyenv
#  https://github.com/pyenv/pyenv
curl https://pyenv.run | bash
pyenv install $(pyenv install --list | grep -E "^\s*3\.[0-9]+\.[0-9]$" | tail -n 1)
pyenv local $(pyenv install --list | grep -E "^\s*3\.[0-9]+\.[0-9]$" | tail -n 1)
pip install --upgrade pip'

# Enable LUKS2 on-the-fly crypto-container generation
sudo apt install cryptsetup

./install_modules.sh
```


### Example usage
```
./test.sh -m 0 -t all
```
All options: `./test.sh --help`
