
## open-bmp

Learning how to work with `.bmp` files

### Открытие файла
```cpp
OpenBMP obmp = OpenBMP("file.bmp");
```

### Получить размерность файла
```cpp
obmp.shape();
```

Размерность файла возвращается в формате `(y, x)` в виде `pair<int, int>`.


