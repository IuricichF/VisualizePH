


cd computePers

cp $1 mesh.ply
g++  -I./source/include/ source/main.cpp -std=c++11 -o computePers

./computePers mesh.ply

mv mesh.ply ../visualizer/model/mesh.ply
mv pairs.json ../visualizer/model/pairs.json

cd ../visualizer

python -m SimpleHTTPServer 8000
