cp -r $RECIPE_DIR/../* .

export CFLAGS="-I$PREFIX/include -L$PREFIX/lib"

make
make install