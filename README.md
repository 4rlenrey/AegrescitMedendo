# AegrescitMedendo



## Bootstraping

```sh
# odb
sudo apt install odb

# crow
cd AegrescitMedendo
cd external 
git clone https://github.com/fnc12/sqlite_orm.git

sudo dpkg -i "$TEMP_DEB"
rm -f "$TEMP_DEB"
```

# Running

```sh
cd AegrescitMedendo/
sudo ./build.sh
```

Your website should be reachable at http://0.0.0.0:80/