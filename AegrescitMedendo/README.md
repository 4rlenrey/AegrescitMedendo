sqlite_orm
```
cd external
wget https://github.com/fnc12/sqlite_orm/archive/refs/tags/v1.8.2.tar.gz
mkdir sqlite_orm
tar -xvzf sqlite_orm-1.8.2.tar.gz sqlite_orm
```

crow:
```
TEMP_DEB="$(mktemp)" &&
wget -O "$TEMP_DEB" 'https://github.com/CrowCpp/Crow/releases/download/v1.0%2B5/crow-v1.0+5.deb' &&
sudo dpkg -i "$TEMP_DEB"
rm -f "$TEMP_DEB"
```


libcppjwt:

```
sudo apt install nlohmann-json3-dev 
sudo apt install libgtest-dev
sudo apt install libssl-dev
sudo apt install libjsoncpp-dev

libcpp-jwt-dev
```

