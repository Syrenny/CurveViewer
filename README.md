# 📈 CurveViewer

Небольшое приложение на Qt6, демонстрирующее работу алгоритма поиска пересечений двух
кривых и визуализацию результатов.

## Функционал

1. Загрузка кривых
2. Отображение участков кривых сплошной или пунктирной линией в зависимости от взаимного
   расположения (кто выше в точке)
3. Переключение режима отображения: сплошная линия/пунктир

## Сборка

```bash
mkdir build && cd build
cmake ..
make
```

## Формат входных данных

Файл должен содержать точки, по одной в строке. Примеры входных файлов находятся в
директории resources/.

Разделителем может быть пробел или табуляция.


