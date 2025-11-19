#  Проверка лексикографической упорядоченности двух строк

- Студент: Тимофеев Никита Сергеевич, группа 3823Б1ПР2
- Технологии: SEQ, MPI
- Вариант: 26

## 1. Вступление
Применение технологии MPI и сравнение производительности программ с использованием MPI и последовательным выполнением, на примере задачи проверки лексикографической упорядоченности двух строк

## 2. Постановка задачи
Задача: на вход программы подаются две строки, необходимо определить лексикографическую упорядоченность каждой из них в отдельности.
Формат входных данных: ```std::pair<std::string, std::string>```
Формат выходных данных: ```std::pair<int, int>```
Необходимо создать две программы, одна из которых использует последовательное выполнение, другая - использует преимущества технологии MPI, затем сравнить их скорость.

## 3. Базовый алгоритм (Последовательный)
```cpp
auto input = GetInput();

// only true if comparison is true on every step
for (size_t i = 0; !input.first.empty() && i < input.first.length() - 1; i++) {
  GetOutput().first &= static_cast<int>(input.first[i] <= input.first[i + 1]);
}
for (size_t i = 0; !input.second.empty() && i < input.second.length() - 1; i++) {
  GetOutput().second &= static_cast<int>(input.second[i] <= input.second[i + 1]);
}

return true;
```

## 4. Схема распараллеливания
- Для MPI: распараллеливание ведётся только относительно двух процессов. Нулевой и первый процесс обрабатывают одним циклом for первую и вторую строку, записывая редультаты в первый и второй элемент пары выходных данных соответственно, затем обмениваясь со всеми остальными процессами результатами с помощью функции MPI_Bcast

## 5. Детали реализации
- Структура кода:
Есть классы ```TimofeevNLexicographicOrderingMPI``` и ```TimofeevNLexicographicOrderingSEQ```, там функция ```bool RunImpl()```. В функции для последовательного исполнения написан вышеприведённый код (раздел 3. Базовый алгоритм (Последовательный)). Код для функции параллельного исполнения приведён в Приложении 1.
- Реализация предполагает, что пустая строка является упорядоченной

## 6. Экспериментальная установка
- Аппаратное обеспечение / ОС: Intel(R) Core(TM) i7-3770, 8 ядер / 1 поток на ядро, 23.4 ГиБ ОЗУ, ОС: кубунту 24.04
- Инструментальная цепочка: gcc (Ubuntu 13.3.0-6ubuntu2~24.04) 13.3.0, clang-tidy-21, clang-format-21, тип сборки: Release
- Данные: тестовые данные для функциональных тестов находятся в файле tests/functional/main.cpp в виде списка типа ``` std::array<TestType, 7>```, тестовые данные для тестирования производительности представляют из себя пару строк, сплошь состоящих из символа 'a', и каждая из них длинной в 500001 символ

## 7. Результаты и выводы

### 7.1 Корректность
Корректность была проверена с помощью функциональных тестов.

### 7.2 Производительность
| Mode         | Time, s        | Speedup |
|--------------|----------------|---------|
| seq, pipeline| 0.1137331963   | 1.00    |
| mpi, pipeline| 0.0882450886   | 1.29    |
| seq, task_run| 0.1136599541   | 1.00    |
| mpi, task_run| 0.0863195980   | 1.32    |
Speedup для mpi = (seq time / mpi time)

## 8. Заключение
Были написаны две программы для сарвнения последовательного выполнения и mpi решения задачи проверки упорядоченности дву строк. В результате применения технологии MPI программа ускорилась.

## Приложение
Приложение 1. Код функции параллельного исполнения
```cpp
bool TimofeevNLexicographicOrderingMPI::RunImpl() {
  auto input = GetInput();

  int rank = 0;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  int size = 0;
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  GetOutput() = std::pair<int, int>(1, 1);

  if (size <= 1) {
    for (size_t i = 0; !input.first.empty() && i < input.first.length() - 1; i++) {
      GetOutput().first &= static_cast<int>(input.first[i] <= input.first[i + 1]);
    }
    for (size_t i = 0; !input.second.empty() && i < input.second.length() - 1; i++) {
      GetOutput().second &= static_cast<int>(input.second[i] <= input.second[i + 1]);
    }
    return true;
  }

  if (rank == 0) {
    // only true if comparison is true on every step
    for (size_t i = 0; !input.first.empty() && i < input.first.length() - 1; i++) {
      GetOutput().first &= static_cast<int>(input.first[i] <= input.first[i + 1]);
    }
  } else if (rank == 1) {
    for (size_t i = 0; !input.second.empty() && i < input.second.length() - 1; i++) {
      GetOutput().second &= static_cast<int>(input.second[i] <= input.second[i + 1]);
    }
  }

  MPI_Bcast(&GetOutput().first, 1, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Bcast(&GetOutput().second, 1, MPI_INT, 1, MPI_COMM_WORLD);

  MPI_Barrier(MPI_COMM_WORLD);

  return true;
}
```