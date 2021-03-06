# Базовый графический-игровой движок
Что вообще происходит в этом репозитории? Здесь я всячески развлекюась и пытаюсь написать основу графического/игрового движка на языке C++ с использованием таких графических API как OpenGL и Vulkan.
По задумке, будет поддерживаться 2 рендерера (OpenGL и Vulkan) и между этими рендерерами можно будет переключаться. Сейчас пишется основа с использованием OpenGL, когда она будет готова - настанет очередь Vulkan.
 
### Фишки, идеи, планы (этап 1)

  - Хотелось бы реализовать удобное ООП-шное управление рендерерами (добавление объектов, удаление, и т.д)
  - Геометрические примитивы движка (статические мешы), с гибкой настройкой отдельных групп полигонов этих мешей (размер текстуры, поворот текстуры, bump, specular, материалы и др.) а так же настройкой самих мешей (положение, поврот, масштаб и прочее)
  - Освещение и тени, с возможностью добавления источников света и их настройкой
  - Динамически-анимированные меши (геоматрия с анимацией)
  - Оптимизация отображаемых объектов и источников света (просчет только необходдимых объектов)
  
Пока что все эти фишки делаются на базе OpenGL. После завершения и тестирования этапа 1, начнется написание Vulkan-Renderer'а и портирование всего этого добра на Vulkan.

Далее будут другие этапы, на котороых пойдет работа над игровой частью и редактором уровней. Но это будет, наверное, настолько не скоро, что пока не вижу смысла пытаться что-то здесь расписывать (я даже толком не знаю что там будет, пока что)

### Что готово на данный момент (19.11.2018)

  - Можно создать ресурсы 3-ех типов (геометрия, текстуры, шейдеры) для дальнейшего их использования (OpenGL)
  - Базовая камера управляемая мышью и клавиатурой, есть функция получения матриц (проекции, вида) для последующей передачи рендереру
  - Можно создать рендерер, настроить матрицы, вызвать метод отрисовки (OpenGL)
  - Можно создать статический меш, настроить его геометрию, текстуры, добавить его в список отрисовки и получить на него указатель, для дальнешего управления (OpenGL)
  - Можно создать источник света нужного типа (точечный, направленный, прожектор), настроить, добавить в список источников и получить указатель для управления (OpenGL)
  
По мере появления новых фишек, буду добавлять их в этот список (впрочем могу об этом забыть, так что если увидите что последний комит был сделан намного позже, чем указанная над списком дата, скорее всего так и произошло). 

В коде, как правило, будет пример использования всех этих функций.

Я тут просто пытаюсь во всем разобраться. Если вы тоже - добро пожаловать!