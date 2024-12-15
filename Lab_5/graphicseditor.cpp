#include "graphicseditor.h"
#include "ui_graphicseditor.h"

GraphicsEditor::GraphicsEditor(QWidget *parent) : QMainWindow(parent),
                                                  ui(new Ui::GraphicsEditor),
                                                  currentColor(Qt::white),
                                                  currentPen(Qt::black),
                                                  topWall(nullptr), bottomWall(nullptr), leftWall(nullptr), rightWall(nullptr), collisionSound(":/res/sound.wav")
{
    ui->setupUi(this);

    scene = new QGraphicsScene(this);
    view = new GraphicsView(scene, this);
    setCentralWidget(view);
    view->setRenderHint(QPainter::Antialiasing);
    view->setRenderHint(QPainter::SmoothPixmapTransform);
    view->setAlignment(Qt::AlignTop | Qt::AlignLeft);                         // Выравнивание области просмотра
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);                // Включаем горизонтальную полосу прокрутки
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);                  // Включаем вертикальную полосу прокрутки
                                                       // Устанавливаем наш view как центральный виджет

    connect(view, &GraphicsView::viewportChanged, this, &GraphicsEditor::updateWallPositions);
    connect(view, &GraphicsView::resized, this, &GraphicsEditor::setupWalls); // Подключение сигнала resized к setupWalls
    setupWalls();

    drawZverev();
    drawLatsuk();
    drawValeria();
    createMovingObject_2();
    createMovingObject_1();
    createMovingObject();

//         Таймер для перемещения объекта
        QTimer *timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, [this]() {
            moveObject(); // Функция перемещения и обнаружения столкновений
        });
        timer->start(30); // Интервал обновления, например, 30 мс
}

GraphicsEditor::~GraphicsEditor()
{
    delete ui;
}

void GraphicsEditor::closeEvent(QCloseEvent *event)
{
    // Удаление всех движущихся объектов
    for (QGraphicsItemGroup *itemGroup : movingItemGroups) {
        QList<QGraphicsItem *> children = itemGroup->childItems();
        for (QGraphicsItem *child : children) {
            scene->removeItem(child);
            delete child;
        }
        scene->removeItem(itemGroup);
        delete itemGroup;
    }
    movingItemGroups.clear();
    velocities.clear();

    // Останавливаем звук
    collisionSound.stop();

    emit editorClosed();
    QMainWindow::closeEvent(event);
}

void GraphicsEditor::on_BackColor_triggered()
{
    QColor color = QColorDialog::getColor(currentColor, this, "Choose Background Color");

    if (color.isValid())
    {
        scene->setBackgroundBrush(color);
        currentColor = color;
    }
}

void GraphicsEditor::createMovingObject()
{
    // Создание нескольких фигур для составного объекта
    QGraphicsRectItem *trunk = new QGraphicsRectItem(17, 30, 15, 75);
    trunk->setBrush(Qt::darkRed);
    QGraphicsEllipseItem *crown = new QGraphicsEllipseItem(0, 0, 50, 75);
    crown->setBrush(Qt::darkGreen);

    // Группируем фигуры в один объект
    QGraphicsItemGroup *tree = new QGraphicsItemGroup();
    tree->addToGroup(trunk);
    tree->addToGroup(crown);

    // Добавляем объект на сцену
    tree->setFlag(QGraphicsItem::ItemIsSelectable, true);
    scene->addItem(tree);

    tree->setPos(600, 300);  // Начальная позиция объекта

    // Добавляем объект и его начальную скорость в соответствующие списки
    movingItemGroups.append(tree);
    velocities.append(QPointF(2, 2)); // Скорость по осям X и Y
    movingStates.append(true);
}

void GraphicsEditor::createMovingObject_1()
{
    // Создание нескольких фигур для составного объекта
    QGraphicsRectItem *body = new QGraphicsRectItem(15, 15, 75, 45);
    body->setBrush(Qt::black);
    QGraphicsRectItem *cab = new QGraphicsRectItem(75, 23, 35, 10);
    cab->setBrush(Qt::lightGray);
    QGraphicsRectItem *smoke = new QGraphicsRectItem(10, 0, 25, 30);
    smoke->setBrush(Qt::lightGray);
    QGraphicsRectItem *wheel1 = new QGraphicsRectItem(25, 40, 15, 30);
    wheel1->setBrush(Qt::lightGray);
    QGraphicsRectItem *wheel2 = new QGraphicsRectItem(60, 40, 15, 30);
    wheel2->setBrush(Qt::lightGray);
    QGraphicsRectItem *nose = new QGraphicsRectItem(16, 13, 12, 12);
    nose->setBrush(Qt::black);
    QGraphicsEllipseItem *Ear1 = new QGraphicsEllipseItem(-5, -4, 20, 10);
    Ear1->setBrush(Qt::darkGray);
    QGraphicsEllipseItem *Ear2 = new QGraphicsEllipseItem(30, -4, 20, 10);
    Ear2->setBrush(Qt::darkGray);

    // Группируем фигуры в один объект
    QGraphicsItemGroup *train = new QGraphicsItemGroup();
    train->addToGroup(body);
    train->addToGroup(cab);
    train->addToGroup(smoke);
    train->addToGroup(wheel1);
    train->addToGroup(wheel2);
    train->addToGroup(nose);
    train->addToGroup(Ear1);
    train->addToGroup(Ear2);

    // Добавляем объект на сцену
    train->setFlag(QGraphicsItem::ItemIsSelectable, true);
    scene->addItem(train);

    train->setPos(400, 500);  // Начальная позиция объекта

    // Добавляем объект и его начальную скорость в соответствующие списки
    movingItemGroups.append(train);
    velocities.append(QPointF(2, 2)); // Скорость по осям X и Y
    movingStates.append(true);
}

void GraphicsEditor::createMovingObject_2()
{
    // Создание тела птицы (овальный корпус)
    QGraphicsEllipseItem *body = new QGraphicsEllipseItem(10, 10, 50, 30);
    body->setBrush(Qt::yellow); // Жёлтый цвет для тела птицы
    body->setPen(QPen(Qt::black));

    // Создание головы птицы (меньший овал)
    QGraphicsEllipseItem *head = new QGraphicsEllipseItem(35, 0, 20, 20);
    head->setBrush(Qt::yellow); // Жёлтый цвет для головы птицы
    head->setPen(QPen(Qt::black));

    // Создание клюва
    QGraphicsPolygonItem *beak = new QGraphicsPolygonItem();
    QPolygonF beakPolygon;
    beakPolygon << QPointF(50, 10) << QPointF(55, 5) << QPointF(55, 15);
    beak->setPolygon(beakPolygon);
    beak->setBrush(Qt::green); // Оранжевый цвет для клюва

    // Создание крыльев (2 оваля)
    QGraphicsEllipseItem *wingLeft = new QGraphicsEllipseItem(0, 10, 30, 15);
    wingLeft->setBrush(Qt::darkGray); // Темно-серый цвет для левого крыла
    wingLeft->setPen(QPen(Qt::black));

    QGraphicsEllipseItem *wingRight = new QGraphicsEllipseItem(20, 10, 30, 15);
    wingRight->setBrush(Qt::darkGray); // Темно-серый цвет для правого крыла
    wingRight->setPen(QPen(Qt::black));

    // Создание хвоста (треугольник)
    QGraphicsPolygonItem *tail = new QGraphicsPolygonItem();
    QPolygonF tailPolygon;
    tailPolygon << QPointF(10, 35) << QPointF(20, 45) << QPointF(0, 45);
    tail->setPolygon(tailPolygon);
    tail->setBrush(Qt::darkGray); // Темно-серый цвет для хвоста

    // Группируем части птицы в один объект
    QGraphicsItemGroup *bird = new QGraphicsItemGroup();
    bird->addToGroup(body);
    bird->addToGroup(head);
    bird->addToGroup(beak);
    bird->addToGroup(wingLeft);
    bird->addToGroup(wingRight);
    bird->addToGroup(tail);

    // Устанавливаем флаг для выбора объекта
    bird->setFlag(QGraphicsItem::ItemIsSelectable, true);

    // Добавляем объект на сцену
    scene->addItem(bird);

    // Устанавливаем начальную позицию птицы
    bird->setPos(700, 500); // Центр экрана

    // Добавляем объект и его начальную скорость в соответствующие списки
    movingItemGroups.append(bird);
    velocities.append(QPointF(2, -2)); // Скорость по осям X и Y
    movingStates.append(true);
}


void GraphicsEditor::moveObject()
{
    int wallThickness = 10;

    for (int i = 0; i < movingItemGroups.size(); ++i) 
    {
        if(movingStates[i]){
          QGraphicsItemGroup *itemGroup = movingItemGroups[i];
          QPointF velocity = velocities[i];
          QPointF newPos = itemGroup->pos() + velocity;
          // Проверка столкновения с левой и правой стенами
          QRectF boundingRect = itemGroup->boundingRect();
          qreal left = newPos.x();
          qreal right = newPos.x() + boundingRect.width();
          qreal top = newPos.y();
          qreal bottom = newPos.y() + boundingRect.height();
                  // Проверка столкновения с левой и правой стенами
                  if (left <= wallThickness) {
                      velocity.setX(-velocity.x());
                      collisionSound.play();
                  } else if (right >= view->viewport()->width() - 2*wallThickness) {
                      velocity.setX(-velocity.x());
                      collisionSound.play();
                  }
                  // Проверка столкновения с верхней и нижней стенами
                  if (top <= wallThickness) {
                      velocity.setY(-velocity.y());
                      collisionSound.play();
                  } else if (bottom >= view->viewport()->height() - 2*wallThickness) {
                      velocity.setY(-velocity.y());
                      collisionSound.play();
                  }

                  // Обработка столкновений с другими объектами
                  bool collisionDetected = false;
                  QList<QGraphicsItem *> itemsAtNewPos = scene->items(QRectF(newPos, boundingRect.size()));
                  for (QGraphicsItem *otherItem : itemsAtNewPos) {
                      if (otherItem != itemGroup && otherItem->data(0) != "user" && otherItem->data(0) != "image") {
                          QRectF otherBoundingRect = otherItem->boundingRect().translated(otherItem->pos());
                          qreal otherLeft = otherBoundingRect.x();
                          qreal otherRight = otherBoundingRect.x() + otherBoundingRect.width();
                          qreal otherTop = otherBoundingRect.y();
                          qreal otherBottom = otherBoundingRect.y() + otherBoundingRect.height();
                          // Проверка пересечения по оси X
                          if (right > otherLeft && left < otherRight) {
                              // Проверка пересечения по оси Y
                              if (bottom > otherTop && top < otherBottom) {
                                  collisionDetected = true;
                                  // Изменение направления скорости (отскок) при столкновении с другим объектом
                                  // Если объект двигается вправо, отражаем скорость по оси X
                                  if (velocity.x() > 0) {
                                      velocity.setX(-velocity.x());
                                  }
                                  // Если объект двигается влево, отражаем скорость по оси X
                                  else if (velocity.x() < 0) {
                                      velocity.setX(-velocity.x());
                                  }
                                  // Если объект двигается вниз, отражаем скорость по оси Y
                                  if (velocity.y() > 0) {
                                      velocity.setY(-velocity.y());
                                  }
                                  // Если объект двигается вверх, отражаем скорость по оси Y
                                  else if (velocity.y() < 0) {
                                      velocity.setY(-velocity.y());
                                  }
                                  // Воспроизведение звука столкновения
                                  collisionSound.play();
                                  break;
                              }
                          }
                      }
                  }
                  // Обновление позиции объекта только после изменения скорости
                  if (collisionDetected) {
                      itemGroup->setPos(newPos);
                  }
          // Обновляем позицию объекта и скорость
          itemGroup->setPos(newPos);
          velocities[i] = velocity;
      }
  
    }
}



void GraphicsEditor::on_SetPen_triggered()
{

    QDialog dialog(this);
    dialog.setWindowTitle(tr("Настройка пера"));

    QVBoxLayout *layout = new QVBoxLayout(&dialog);
    QFormLayout *formLayout = new QFormLayout();

    // Выбор стиля пера
    QComboBox *styleComboBox = new QComboBox();
    styleComboBox->setIconSize(QSize(64, 64)); // Например, 32x32 пикселя
    styleComboBox->addItem(QIcon(":/res/images/penIcons/Solid"), "Solid", QVariant::fromValue(Qt::SolidLine));
    styleComboBox->addItem(QIcon(":/res/images/penIcons/Dash"), "Dash", QVariant::fromValue(Qt::DashLine));
    styleComboBox->addItem(QIcon(":/res/images/penIcons/DotLine"), "Dot", QVariant::fromValue(Qt::DotLine));
    styleComboBox->addItem(QIcon(":/res/images/penIcons/DashDotLine"), "Dash Dot", QVariant::fromValue(Qt::DashDotLine));
    styleComboBox->addItem(QIcon(":/res/images/penIcons/DashDotDotLine"), "Dash Dot Dot", QVariant::fromValue(Qt::DashDotDotLine));
    styleComboBox->setCurrentIndex(styleComboBox->findData(static_cast<int>(currentPen.style())));
    formLayout->addRow(tr("Стиль:"), styleComboBox);

    // Выбор ширины пера
    QSpinBox *widthSpinBox = new QSpinBox();
    widthSpinBox->setRange(1, 20);
    widthSpinBox->setValue(currentPen.width()); // начальное значение
    formLayout->addRow(tr("Ширина:"), widthSpinBox);

    // Выбор цвета
    QPushButton *colorButton = new QPushButton(tr("Выбрать цвет"));
    QColor penColor = currentPen.color(); // Начальный цвет
    colorButton->setStyleSheet(QString("background-color: %1").arg(penColor.name()));
    connect(colorButton, &QPushButton::clicked, [&]()
            {
        QColor color = QColorDialog::getColor(penColor, this, tr("Выберите цвет пера"));
        if (color.isValid()) {
            penColor = color;
            colorButton->setStyleSheet(QString("background-color: %1").arg(color.name())); // Изменение цвета кнопки
        } });
    formLayout->addRow(tr("Цвет:"), colorButton);

    // Выбор стиля конца
    QComboBox *capStyleComboBox = new QComboBox();
    capStyleComboBox->setIconSize(QSize(64, 64));
    capStyleComboBox->addItem(QIcon(":/res/images/penIcons/FlatCap"), "Flat", QVariant::fromValue(Qt::FlatCap));
    capStyleComboBox->addItem(QIcon(":/res/images/penIcons/RoundCap"), "Round", QVariant::fromValue(Qt::RoundCap));
    capStyleComboBox->addItem(QIcon(":/res/images/penIcons/SquareCap"), "Square", QVariant::fromValue(Qt::SquareCap));
    capStyleComboBox->setCurrentIndex(capStyleComboBox->findData(static_cast<int>(currentPen.capStyle())));
    formLayout->addRow(tr("Стиль конца:"), capStyleComboBox);

    // Выбор стиля соединения
    QComboBox *joinStyleComboBox = new QComboBox();
    joinStyleComboBox->setIconSize(QSize(64, 64));
    joinStyleComboBox->addItem(QIcon(":/res/images/penIcons/MilterJoin"), "Miter", QVariant::fromValue(Qt::MiterJoin));
    joinStyleComboBox->addItem(QIcon(":/res/images/penIcons/BevelJoin"), "Bevel", QVariant::fromValue(Qt::BevelJoin));
    joinStyleComboBox->addItem(QIcon(":/res/images/penIcons/RoundJoin"), "Round", QVariant::fromValue(Qt::RoundJoin));
    joinStyleComboBox->setCurrentIndex(joinStyleComboBox->findData(static_cast<int>(currentPen.joinStyle())));
    formLayout->addRow(tr("Стиль соединения:"), joinStyleComboBox);

    layout->addLayout(formLayout);

    // Кнопка ОК
    QPushButton *okButton = new QPushButton(tr("ОК"));
    layout->addWidget(okButton);

    connect(okButton, &QPushButton::clicked, [&]()
            {
                // Установка пера с выбранными параметрами
                currentPen.setStyle(static_cast<Qt::PenStyle>(styleComboBox->currentData().value<int>()));
                currentPen.setWidth(widthSpinBox->value());
                currentPen.setColor(penColor);
                currentPen.setCapStyle(static_cast<Qt::PenCapStyle>(capStyleComboBox->currentData().value<int>()));
                currentPen.setJoinStyle(static_cast<Qt::PenJoinStyle>(joinStyleComboBox->currentData().value<int>()));

                view->setPen(currentPen); // Передаем перо в GraphicsView
                dialog.accept();          // Закрыть диалог
            });

    dialog.exec(); // Показать диалог
    view->setEraserMode(false);
}

void GraphicsEditor::on_Clear_triggered()
{
    // Останавливаем движение всех объектов (если они двигаются)
    for (int i = 0; i < movingItemGroups.size(); ++i) {
        // Останавливаем все анимации или действия, связанные с движущимися объектами
        movingItemGroups[i]->setPos(QPointF(0, 0));  // Пример остановки движения
    }

    QList<QGraphicsItem *> items = scene->items();
    // Удаляем все объекты, кроме стен
    foreach (QGraphicsItem *item, items) {
        // Проверяем, что это не стена
        if (item != topWall && item != bottomWall && item != leftWall && item != rightWall) {
            scene->removeItem(item);  // Убираем из сцены
            movingItemGroups.removeAll(qgraphicsitem_cast<QGraphicsItemGroup*>(item));
            delete item;              // Немедленное удаление объекта
        }
    }

    // Опционально можно перерисовать стены, чтобы они точно остались на месте
    setupWalls();

    scene->setBackgroundBrush(Qt::white);  // Сброс фона (если нужно)
}

void GraphicsEditor::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    setupWalls(); // Перестраиваем стены при каждом изменении размера окна
}

void GraphicsEditor::setupWalls()
{
    int viewWidth = view->viewport()->width();
    int viewHeight = view->viewport()->height();
    int wallThickness = 10;

    QPixmap wallImage(":/res/images/wall.jpg");

    QPixmap scaledTopBottom = wallImage.scaled(viewWidth, wallThickness);
        QPixmap scaledLeftRight = wallImage.scaled(wallThickness, viewHeight);

    // Если стены ещё не были созданы, создаём их, иначе просто изменяем размеры
    if (!topWall)
    {
        topWall = scene->addPixmap(scaledTopBottom);
                bottomWall = scene->addPixmap(scaledTopBottom);
                leftWall = scene->addPixmap(scaledLeftRight);
                rightWall = scene->addPixmap(scaledLeftRight);

        topWall->setFlag(QGraphicsItem::ItemIsMovable, false);
        bottomWall->setFlag(QGraphicsItem::ItemIsMovable, false);
        leftWall->setFlag(QGraphicsItem::ItemIsMovable, false);
        rightWall->setFlag(QGraphicsItem::ItemIsMovable, false);
    }
    else
    {
        topWall->setPixmap(scaledTopBottom);
                bottomWall->setPixmap(scaledTopBottom);
                leftWall->setPixmap(scaledLeftRight);
                rightWall->setPixmap(scaledLeftRight);
    }

    updateWallPositions();
}

void GraphicsEditor::updateWallPositions()
{
    int wallThickness = 10;

    int horizontalOffset = view->horizontalScrollBar()->value();
    int verticalOffset = view->verticalScrollBar()->value();

    if (topWall)
        topWall->setPos(horizontalOffset, verticalOffset);
    if (bottomWall)
        bottomWall->setPos(horizontalOffset, view->viewport()->height() - wallThickness + verticalOffset);
    if (leftWall)
        leftWall->setPos(horizontalOffset, verticalOffset);
    if (rightWall)
        rightWall->setPos(view->viewport()->width() - wallThickness + horizontalOffset, verticalOffset);
}

void GraphicsEditor::on_AddFigure_triggered()
{
    QDialog dialog(this);
    dialog.setWindowTitle(tr("Добавить фигуру"));

    QVBoxLayout *layout = new QVBoxLayout(&dialog);
    QFormLayout *formLayout = new QFormLayout();

    // Выбор типа фигуры
    QComboBox *shapeComboBox = new QComboBox();
    shapeComboBox->addItem("Квадрат", "Square");
    shapeComboBox->addItem("Прямоугольник", "Rectangle");
    shapeComboBox->addItem("Треугольник", "Triangle");
    shapeComboBox->addItem("Круг", "Circle");
    shapeComboBox->addItem("Эллипс", "Ellipse");
    formLayout->addRow(tr("Тип фигуры:"), shapeComboBox);

    // Ввод размера (ширина и высота)
    QSpinBox *widthSpinBox = new QSpinBox();
    widthSpinBox->setRange(1, 1000);
    widthSpinBox->setValue(100);
    formLayout->addRow(tr("Ширина:"), widthSpinBox);

    QSpinBox *heightSpinBox = new QSpinBox();
    heightSpinBox->setRange(1, 1000);
    heightSpinBox->setValue(100);
    formLayout->addRow(tr("Высота:"), heightSpinBox);

    // Функция, которая будет изменять поля в зависимости от выбранной фигуры
    auto updateShapeInputs = [&]() {
        QString shapeType = shapeComboBox->currentData().toString();

        if (shapeType == "Square") {
            heightSpinBox->setValue(widthSpinBox->value());  // Устанавливаем высоту равной ширине
            heightSpinBox->setEnabled(false);  // Выключаем поле высоты
        } else if (shapeType == "Circle") {
            heightSpinBox->setValue(widthSpinBox->value());  // Устанавливаем высоту равной ширине для радиуса
            heightSpinBox->setEnabled(false);  // Выключаем поле высоты
        } else {
            heightSpinBox->setEnabled(true);  // Включаем поле высоты для других фигур
        }
    };

    // Обновление полей при изменении типа фигуры
    connect(shapeComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), updateShapeInputs);


    // Изначально вызываем функцию для корректного отображения полей
    updateShapeInputs();

    // Выбор цвета заливки
    QPushButton *fillColorButton = new QPushButton(tr("Цвет заливки"));
    QColor fillColor = Qt::darkBlue;
    fillColorButton->setStyleSheet(QString("background-color: %1").arg(fillColor.name()));
    connect(fillColorButton, &QPushButton::clicked, [&]() {
        QColor color = QColorDialog::getColor(fillColor, this, tr("Выберите цвет заливки"));
        if (color.isValid()) {
            fillColor = color;
            fillColorButton->setStyleSheet(QString("background-color: %1").arg(color.name()));
        }
    });
    formLayout->addRow(tr("Цвет заливки:"), fillColorButton);

    // Выбор цвета обводки
    QPushButton *strokeColorButton = new QPushButton(tr("Цвет обводки"));
    QColor strokeColor = Qt::black;
    strokeColorButton->setStyleSheet(QString("background-color: %1").arg(strokeColor.name()));
    connect(strokeColorButton, &QPushButton::clicked, [&]() {
        QColor color = QColorDialog::getColor(strokeColor, this, tr("Выберите цвет обводки"));
        if (color.isValid()) {
            strokeColor = color;
            strokeColorButton->setStyleSheet(QString("background-color: %1").arg(color.name()));
        }
    });
    formLayout->addRow(tr("Цвет обводки:"), strokeColorButton);

    // Выбор ширины обводки
    QSpinBox *strokeWidthSpinBox = new QSpinBox();
    strokeWidthSpinBox->setRange(1, 20);
    strokeWidthSpinBox->setValue(2);
    formLayout->addRow(tr("Ширина обводки:"), strokeWidthSpinBox);

    QComboBox *brushComboBox = new QComboBox();
    brushComboBox->addItem("Кисть не задана", "NoBrush");
    brushComboBox->addItem("Однородный цвет", "SolidPattern");
    brushComboBox->addItem("Чрезвычайно плотная кисть", "Dense1Pattern");
    brushComboBox->addItem("Очень плотная кисть", "Dense2Pattern");
    brushComboBox->addItem("Довольно плотная кисть", "Dense3Pattern");
    brushComboBox->addItem("Наполовину плотная кисть", "Dense4Pattern");
    brushComboBox->addItem("Довольно редкая кисть", "Dense5Pattern");
    brushComboBox->addItem("Очень редкая кисть", "Dense6Pattern");
    brushComboBox->addItem("Чрезвычайно редкая кисть", "Dense7Pattern");
    brushComboBox->addItem("Горизонтальные линии", "HorPattern");
    brushComboBox->addItem("Вертикальные линии", "VerPattern");
    brushComboBox->addItem("Пересекающиеся вертикальные и горизонтальные линии", "CrossPattern");
    brushComboBox->addItem("Обратные диагональные линии", "BDiagPattern");
    brushComboBox->addItem("Прямые диагональные линии", "FDiagPattern");
    brushComboBox->addItem("Пересекающиеся диагональные линии", "DiagCrossPattern");
    formLayout->addRow(tr("Тип заливки:"), brushComboBox);

    layout->addLayout(formLayout);

    // Кнопка ОК
    QPushButton *okButton = new QPushButton(tr("ОК"));
    layout->addWidget(okButton);

    connect(okButton, &QPushButton::clicked, [&]() {
        // Получаем значения из формы
        QString shapeType = shapeComboBox->currentData().toString();
        int width = widthSpinBox->value();
        int height = (shapeType == "Square" || shapeType == "Circle") ? width : heightSpinBox->value();
        int x = (view->viewport()->width() - width) / 2;  // Центрируем фигуру
        int y = (view->viewport()->height() - height) / 2;

        QString brushStyleStr = brushComboBox->currentData().toString();
        Qt::BrushStyle brushStyle = stringToBrushStyle(brushStyleStr);

        // Добавляем фигуру
        addShape(shapeType, QRectF(x, y, width, height), fillColor, brushStyle, strokeColor, strokeWidthSpinBox->value());

        dialog.accept(); // Закрыть диалог
    });

    dialog.exec(); // Показать диалог
}


void GraphicsEditor::addShape(QString shapeType, QRectF rect, QColor fillColor, Qt::BrushStyle brushStyle, QColor strokeColor, int strokeWidth)
{
    QGraphicsItem *shape = nullptr;
    QPen pen(strokeColor, strokeWidth);
    QBrush brush(fillColor, brushStyle);

    if (shapeType == "Square") {
        // Квадрат (равные ширина и высота)
        shape = scene->addRect(rect, pen, brush);
    } else if (shapeType == "Rectangle") {
        // Прямоугольник
        shape = scene->addRect(rect, pen, brush);
    } else if (shapeType == "Circle") {
        // Круг
        shape = scene->addEllipse(rect, pen, brush);
    } else if (shapeType == "Ellipse") {
        // Эллипс
        shape = scene->addEllipse(rect, pen, brush);
    } else if (shapeType == "Triangle") {
        // Треугольник
        QPolygonF triangle;
        triangle << QPointF(rect.left() + rect.width() / 2, rect.top())
                 << QPointF(rect.bottomLeft())
                 << QPointF(rect.bottomRight());
        shape = scene->addPolygon(triangle, pen, brush);
    }

        shape->setFlag(QGraphicsItem::ItemIsMovable, true); // Фигуры можно перемещать
        shape->setFlag(QGraphicsItem::ItemIsSelectable, true); // Фигуры можно выделять
        shape->setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);

}

void GraphicsEditor::on_DeleteFigure_triggered()
{
    // Получаем список всех выбранных объектов на сцене
    QList<QGraphicsItem *> selectedItems = scene->selectedItems();

    // Проверка, есть ли вообще выбранные элементы
    if (selectedItems.isEmpty()) {
        qDebug() << "No items selected for deletion.";
        return;
    }

    // Выводим количество выбранных объектов
    qDebug() << "Selected items count:" << selectedItems.count();

    foreach (QGraphicsItem *item, selectedItems) {
        qDebug() << "Item type:" << item->type();

        if (QGraphicsItemGroup *group = qgraphicsitem_cast<QGraphicsItemGroup *>(item)) {
            // Если это группа, выводим информацию и удаляем её элементы
            qDebug() << "Removing group with child items.";

            movingItemGroups.removeAll(group);

            QList<QGraphicsItem *> children = group->childItems();
            for (QGraphicsItem *child : children) {
                qDebug() << "Removing child item:" << child;
                scene->removeItem(child);  // Убираем элемент из сцены
                delete child;              // Удаляем элемент
            }

            scene->removeItem(group);  // Убираем саму группу из сцены
            delete group;              // Удаляем саму группу
        } else {
            // Если это не группа, удаляем только объект
            qDebug() << "Removing single item:" << item;
            scene->removeItem(item);  // Убираем объект из сцены
            delete item;              // Удаляем объект
        }
    }

    // Обновляем сцену и выводим сообщение
    scene->update();
    qDebug() << "Scene updated after deletion.";
}

void GraphicsEditor::drawZverev() {
    //  Z
    QGraphicsItemGroup *group_Z = new QGraphicsItemGroup();
    QGraphicsItem *Z_1 = scene->addLine(QLineF(20, 20, 50, 20), QPen(Qt::blue, 6));
    group_Z->addToGroup(Z_1);
    QGraphicsItem *Z_2 = scene->addLine(QLineF(50, 20, 20, 60), QPen(Qt::blue, 6));
    group_Z->addToGroup(Z_2);
    QGraphicsItem *Z_3 = scene->addLine(QLineF(20, 60, 50, 60), QPen(Qt::blue, 6));
    group_Z->addToGroup(Z_3);
    groupSetFlags(group_Z);
    scene->addItem(group_Z);

    //  V
    QGraphicsItemGroup *group_V = new QGraphicsItemGroup();
    QGraphicsItem *V_1 = scene->addLine(QLineF(50, 20, 65, 60), QPen(Qt::blue, 6));
    group_V->addToGroup(V_1);
    QGraphicsItem *V_2 = scene->addLine(QLineF(65, 60, 75, 20), QPen(Qt::blue, 6));
    group_V->addToGroup(V_2);
    groupSetFlags(group_V);
    scene->addItem(group_V);

    //  E
    QGraphicsItemGroup *group_C = new QGraphicsItemGroup();
    QGraphicsLineItem *C_1 = scene->addLine(QLineF(90, 15, 120, 15), QPen(Qt::darkBlue, 6));
    group_C->addToGroup(C_1);
    QGraphicsLineItem *C_2 = scene->addLine(QLineF(90, 15, 90, 60), QPen(Qt::darkBlue, 6));
    group_C->addToGroup(C_2);
    QGraphicsLineItem *C_3 = scene->addLine(QLineF(90, 60, 120, 60), QPen(Qt::darkBlue, 6));
    group_C->addToGroup(C_3);
    QGraphicsLineItem *C_4 = scene->addLine(QLineF(90, 40, 120, 40), QPen(Qt::darkBlue, 6));
    group_C->addToGroup(C_4);
    groupSetFlags(group_C);

    scene->addItem(group_C);

    // R
    QGraphicsItemGroup *group_R = new QGraphicsItemGroup();
    QGraphicsItem *R_1 = scene->addRect(QRectF(140, 20, 30, 10), QPen(Qt::green, 2), QBrush(Qt::black, Qt::SolidPattern));
    group_R->addToGroup(R_1);
    QGraphicsItem *R_2 = scene->addLine(QLineF(140, 20, 160, 20), QPen(Qt::green, 2));
    group_R->addToGroup(R_2);
    QGraphicsItem *R_4 = scene->addLine(QLineF(140, 20, 140, 60), QPen(Qt::green, 2));
    group_R->addToGroup(R_4);
    QGraphicsItem *R_3 = scene->addLine(QLineF(140, 20, 170, 50), QPen(Qt::green, 2));
    group_R->addToGroup(R_3);
    groupSetFlags(group_R);

    scene->addItem(group_R);

    //  E
    QGraphicsItemGroup *group_CQ = new QGraphicsItemGroup();
    QGraphicsLineItem *CQ_1 = scene->addLine(QLineF(195, 15, 225, 15), QPen(Qt::red, 6));
    group_CQ->addToGroup(CQ_1);
    QGraphicsLineItem *CQ_2 = scene->addLine(QLineF(195, 15, 195, 60), QPen(Qt::red, 6));
    group_CQ->addToGroup(CQ_2);
    QGraphicsLineItem *CQ_3 = scene->addLine(QLineF(195, 60, 225, 60), QPen(Qt::red, 6));
    group_CQ->addToGroup(CQ_3);
    QGraphicsLineItem *CQ_4 = scene->addLine(QLineF(195, 40, 225, 40), QPen(Qt::red, 6));
    group_CQ->addToGroup(CQ_4);
    groupSetFlags(group_CQ);

    scene->addItem(group_CQ);

    //  остальные буквы при помощи шрифта
    QFont font("Arial", 50, QFont::Bold);
    QGraphicsTextItem *textV1 = new QGraphicsTextItem("В");
    textV1->setFont(font);
    textV1->setDefaultTextColor(Qt::green);
    textV1->setPos(240, 15);
    textSetFlags(textV1);
    scene->addItem(textV1);

    QGraphicsTextItem *textM1 = new QGraphicsTextItem("М");
    textM1->setFont(font);
    textM1->setDefaultTextColor(Qt::green);
    textM1->setPos(290, 15);
    textSetFlags(textM1);
    scene->addItem(textM1);

    QGraphicsTextItem *textI1 = new QGraphicsTextItem("И");
    textI1->setFont(font);
    textI1->setDefaultTextColor(Qt::green);
    textI1->setPos(340, 15);
    textSetFlags(textI1);
    scene->addItem(textI1);

    QGraphicsTextItem *textH1 = new QGraphicsTextItem("Х");
    textH1->setFont(font);
    textH1->setDefaultTextColor(Qt::green);
    textH1->setPos(390, 15);
    textSetFlags(textH1);
    scene->addItem(textH1);

    QGraphicsTextItem *textA1 = new QGraphicsTextItem("А");
    textA1->setFont(font);
    textA1->setDefaultTextColor(Qt::green);
    textA1->setPos(440, 15);
    textSetFlags(textA1);
    scene->addItem(textA1);

    QGraphicsTextItem *textI2 = new QGraphicsTextItem("И");
    textI2->setFont(font);
    textI2->setDefaultTextColor(Qt::green);
    textI2->setPos(490, 15);
    textSetFlags(textI2);
    scene->addItem(textI2);

    QGraphicsTextItem *textL = new QGraphicsTextItem("Л");
    textL->setFont(font);
    textL->setDefaultTextColor(Qt::green);
    textL->setPos(540, 15);
    textSetFlags(textL);
    scene->addItem(textL);

    scene->update();  // Обновить всю сцену


}

void GraphicsEditor::drawLatsuk() {
    //  Л
    QGraphicsItemGroup *group_L = new QGraphicsItemGroup();
    QGraphicsItem *L_1 = scene->addRect(QRectF(15,15,50,10), QPen(Qt::green, 2), QBrush(Qt::black, Qt::SolidPattern));
    L_1->setRotation(75);
    L_1->setPos(60, 100);
    group_L->addToGroup(L_1);
    QGraphicsItem *L_2 = scene->addRect(QRectF(15,15,50,10), QPen(Qt::green, 2), QBrush(Qt::black, Qt::SolidPattern));
    L_2->setRotation(-75);
    L_2->setPos(5, 180);
    group_L->addToGroup(L_2);
    groupSetFlags(group_L);
    scene->addItem(group_L);

    //  A
    QGraphicsItemGroup *group_A = new QGraphicsItemGroup();
    QGraphicsItem *A_1 = scene->addLine(QLineF(70, 160, 85, 115), QPen(Qt::red, 6));
    group_A->addToGroup(A_1);
    QGraphicsItem *A_2 = scene->addLine(QLineF(85, 115, 100, 160), QPen(Qt::red, 6));
    group_A->addToGroup(A_2);
    QGraphicsItem *A_3 = scene->addLine(QLineF(75, 150, 95, 150), QPen(Qt::red, 6));
    group_A->addToGroup(A_3);
    groupSetFlags(group_A);

    scene->addItem(group_A);

    //  Ц
    QGraphicsItemGroup *group_C = new QGraphicsItemGroup();
    QGraphicsItem *C_1 = scene->addLine(QLineF(110, 160, 110, 115), QPen(Qt::blue, 6));
    group_C->addToGroup(C_1);
    QGraphicsItem *C_2 = scene->addRect(QRectF(110,155,30,10), QPen(Qt::blue, 2), QBrush(Qt::yellow, Qt::SolidPattern));
    group_C->addToGroup(C_2);
    QGraphicsItem *C_3 = scene->addLine(QLineF(140, 115, 140, 160), QPen(Qt::blue, 6));
    group_C->addToGroup(C_3);
    QGraphicsItem *C_4 = scene->addRect(QRectF(155,-145,20,8), QPen(Qt::blue, 2), QBrush(Qt::yellow, Qt::SolidPattern));
    C_4->setRotation(90);
    group_C->addToGroup(C_4);
    groupSetFlags(group_C);

    scene->addItem(group_C);

    //  У
    QGraphicsItemGroup *group_U = new QGraphicsItemGroup();
    QGraphicsItem *U_1 = scene->addLine(QLineF(150, 115, 160, 130), QPen(Qt::red, 6));
    group_U->addToGroup(U_1);
    QGraphicsItem *U_2 = scene->addLine(QLineF(150, 160, 170, 115), QPen(Qt::red, 6));
    group_U->addToGroup(U_2);
    groupSetFlags(group_U);

    scene->addItem(group_U);

    //  K
    QGraphicsItemGroup *group_K = new QGraphicsItemGroup();
    QGraphicsItem *K_1 = scene->addRect(QRectF(185,115,10,50), QPen(Qt::green, 2), QBrush(Qt::black, Qt::SolidPattern));
    group_K->addToGroup(K_1);
    QGraphicsItem *K_2 = scene->addRect(QRectF(135,135,30,10), QPen(Qt::green, 2), QBrush(Qt::black, Qt::SolidPattern));
    K_2->setTransformOriginPoint(15, 15);
    K_2->setRotation(-45);
    K_2->setPos(5, 120);
    group_K->addToGroup(K_2);
    QGraphicsItem *K_3 = scene->addRect(QRectF(135,-115,30,10), QPen(Qt::green, 2), QBrush(Qt::black, Qt::SolidPattern));
    K_3->setRotation(45);
    K_3->setPos(15, 120);
    group_K->addToGroup(K_3);
    groupSetFlags(group_K);

    scene->addItem(group_K);

    //  остальные буквы при помощи шрифта
    QFont font("Arial", 50, QFont::Bold);
    QGraphicsTextItem *textA = new QGraphicsTextItem("А");
    textA->setFont(font);
    textA->setDefaultTextColor(Qt::green);
    textA->setPos(220, 115);
    textSetFlags(textA);
    scene->addItem(textA);

    QGraphicsTextItem *textN = new QGraphicsTextItem("Н");
    textN->setFont(font);
    textN->setDefaultTextColor(Qt::green);
    textN->setPos(270, 115);
    textSetFlags(textN);
    scene->addItem(textN);

    QGraphicsTextItem *textD = new QGraphicsTextItem("Д");
    textD->setFont(font);
    textD->setDefaultTextColor(Qt::green);
    textD->setPos(320, 115);
    textSetFlags(textD);
    scene->addItem(textD);

    QGraphicsTextItem *textR = new QGraphicsTextItem("Р");
    textR->setFont(font);
    textR->setDefaultTextColor(Qt::green);
    textR->setPos(370, 115);
    textSetFlags(textR);
    scene->addItem(textR);

    QGraphicsTextItem *textE = new QGraphicsTextItem("Е");
    textE->setFont(font);
    textE->setDefaultTextColor(Qt::green);
    textE->setPos(420, 115);
    textSetFlags(textE);
    scene->addItem(textE);

    QGraphicsTextItem *textY = new QGraphicsTextItem("Й");
    textY->setFont(font);
    textY->setDefaultTextColor(Qt::green);
    textY->setPos(470, 115);
    textSetFlags(textY);
    scene->addItem(textY);

    scene->update();  // Обновить всю сцену
}

void GraphicsEditor::drawValeria() {
    // П
    QGraphicsItemGroup *group_P = new QGraphicsItemGroup();
    QGraphicsItem *P_1 = scene->addLine(QLineF(15,215,15,250), QPen(Qt::magenta, 6));
    group_P->addToGroup(P_1);
    QGraphicsItem *P_2 = scene->addRect(QRectF(15,215,30,10), QPen(Qt::magenta, 2), QBrush(Qt::darkMagenta, Qt::SolidPattern));
    group_P->addToGroup(P_2);
    QGraphicsItem *P_3 = scene->addLine(QLineF(45, 215, 45, 250), QPen(Qt::magenta, 6));
    group_P->addToGroup(P_3);
    groupSetFlags(group_P);

    scene->addItem(group_P);

    // A
    QGraphicsItemGroup *group_A = new QGraphicsItemGroup();
    QGraphicsItem *A_1 = scene->addLine(QLineF(70, 260, 85, 215), QPen(Qt::darkMagenta, 6));
    group_A->addToGroup(A_1);
    QGraphicsItem *A_2 = scene->addLine(QLineF(85, 215, 100, 260), QPen(Qt::darkMagenta, 6));
    group_A->addToGroup(A_2);
    QGraphicsItem *A_3 = scene->addLine(QLineF(75, 250, 95, 250), QPen(Qt::darkMagenta, 6));
    group_A->addToGroup(A_3);
    groupSetFlags(group_A);

    scene->addItem(group_A);

    // Н
    QGraphicsItemGroup *group_N = new QGraphicsItemGroup();
    QGraphicsItem *N_1 = scene->addLine(QLineF(110, 260, 110, 215), QPen(Qt::magenta, 6));
    group_N->addToGroup(N_1);
    QGraphicsItem *N_2 = scene->addRect(QRectF(110,230,30,10), QPen(Qt::magenta, 2), QBrush(Qt::darkMagenta, Qt::SolidPattern));
    group_N->addToGroup(N_2);
    QGraphicsItem *N_3 = scene->addLine(QLineF(140, 215, 140, 260), QPen(Qt::magenta, 6));
    group_N->addToGroup(N_3);
    groupSetFlags(group_N);

    scene->addItem(group_N);

    // Ч
    QGraphicsItemGroup *group_C = new QGraphicsItemGroup();

    // Рисуем первую вертикальную линию
    QGraphicsItem *C_1 = scene->addLine(QLineF(150, 215, 150, 240), QPen(Qt::red, 6));
    group_C->addToGroup(C_1);

    // Рисуем вторую вертикальную линию
    QGraphicsItem *C_2 = scene->addLine(QLineF(170, 215, 170, 260), QPen(Qt::red, 6));
    group_C->addToGroup(C_2);

    // Рисуем диагональную линию, соединяющую верхние концы вертикальных линий
    QGraphicsItem *C_3 = scene->addLine(QLineF(150, 240, 165, 240), QPen(Qt::red, 6));
    group_C->addToGroup(C_3);

    // Устанавливаем флаги для группы
    groupSetFlags(group_C);

    // Добавляем группу на сцену
    scene->addItem(group_C);

    // И
    QGraphicsItemGroup *group_I = new QGraphicsItemGroup();
    QGraphicsLineItem *I_1 = scene->addLine(QLineF(180, 215, 180, 270), QPen(Qt::darkMagenta, 6));
    group_I->addToGroup(I_1);
    QGraphicsLineItem *I_2 = scene->addLine(QLineF(180, 270, 210, 215), QPen(Qt::darkMagenta, 6));
    group_I->addToGroup(I_2);
    QGraphicsLineItem *I_3 = scene->addLine(QLineF(210, 215, 210, 270), QPen(Qt::darkMagenta, 6));
    group_I->addToGroup(I_3);
    groupSetFlags(group_I);

    scene->addItem(group_I);

    // Остальные буквы при помощи шрифта
    QFont font("Arial", 50, QFont::Bold);
    QGraphicsTextItem *textN1 = new QGraphicsTextItem("Н");
    textN1->setFont(font);
    textN1->setDefaultTextColor(Qt::magenta);
    textN1->setPos(220, 215); // Сдвигаем вниз на 200 пикселей
    textSetFlags(textN1);
    scene->addItem(textN1);

    QGraphicsTextItem *textA1 = new QGraphicsTextItem("А");
    textA1->setFont(font);
    textA1->setDefaultTextColor(Qt::green);
    textA1->setPos(270, 215); // Сдвигаем вниз на 200 пикселей
    textSetFlags(textA1);
    scene->addItem(textA1);

    QGraphicsTextItem *textL = new QGraphicsTextItem("Л");
    textL->setFont(font);
    textL->setDefaultTextColor(Qt::magenta);
    textL->setPos(320, 215); // Сдвигаем вниз на 200 пикселей
    textSetFlags(textL);
    scene->addItem(textL);

    QGraphicsTextItem *textE = new QGraphicsTextItem("Е");
    textE->setFont(font);
    textE->setDefaultTextColor(Qt::green);
    textE->setPos(370, 215); // Сдвигаем вниз на 200 пикселей
    textSetFlags(textE);
    scene->addItem(textE);

    QGraphicsTextItem *textR = new QGraphicsTextItem("Р");
    textR->setFont(font);
    textR->setDefaultTextColor(Qt::magenta);
    textR->setPos(420, 215); // Сдвигаем вниз на 200 пикселей
    textSetFlags(textR);
    scene->addItem(textR);

    QGraphicsTextItem *textA2 = new QGraphicsTextItem("А");
    textA2->setFont(font);
    textA2->setDefaultTextColor(Qt::green);
    textA2->setPos(470, 215); // Сдвигаем вниз на 200 пикселей
    textSetFlags(textA2);
    scene->addItem(textA2);

    scene->update();  // Обновить всю сцену
}

void GraphicsEditor::groupSetFlags(QGraphicsItemGroup *group){
    group->setFlag(QGraphicsItem::ItemIsMovable, true);
    group->setFlag(QGraphicsItem::ItemIsSelectable, true);
    group->setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
}

void GraphicsEditor::textSetFlags(QGraphicsTextItem *item){
        item->setFlag(QGraphicsItem::ItemIsMovable, true);
        item->setFlag(QGraphicsItem::ItemIsSelectable, true);
        item->setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
}

Qt::BrushStyle GraphicsEditor::stringToBrushStyle(const QString &styleStr) {
    if (styleStr == "SolidPattern") {
        return Qt::SolidPattern;
    } else if (styleStr == "Dense1Pattern") {
        return Qt::Dense1Pattern;
    } else if (styleStr == "Dense2Pattern") {
        return Qt::Dense2Pattern;
    } else if (styleStr == "Dense3Pattern") {
        return Qt::Dense3Pattern;
    } else if (styleStr == "Dense4Pattern") {
        return Qt::Dense4Pattern;
    } else if (styleStr == "Dense5Pattern") {
        return Qt::Dense5Pattern;
    } else if (styleStr == "Dense6Pattern") {
        return Qt::Dense6Pattern;
    } else if (styleStr == "Dense7Pattern") {
        return Qt::Dense7Pattern;
    } else if (styleStr == "HorPattern") {
        return Qt::HorPattern;
    } else if (styleStr == "VerPattern") {
        return Qt::VerPattern;
    } else if (styleStr == "CrossPattern") {
        return Qt::CrossPattern;
    } else if (styleStr == "BDiagPattern") {
        return Qt::BDiagPattern;
    } else if (styleStr == "FDiagPattern") {
        return Qt::FDiagPattern;
    } else if (styleStr == "DiagCrossPattern") {
        return Qt::DiagCrossPattern;
    } else {
        return Qt::NoBrush;  // Default if no match
    }
}

void GraphicsEditor::on_Eraser_triggered()
{QDialog dialog(this);
    dialog.setWindowTitle(tr("Размер ластика"));

    // Create label, spin box, and OK button for the dialog
    QLabel *label = new QLabel(tr("Выбери размер ластика:"), &dialog);
    QSpinBox *sizeSpinBox = new QSpinBox(&dialog);
    sizeSpinBox->setRange(1, 100);      // Set range for eraser size
    sizeSpinBox->setValue(10);          // Default size

    QPushButton *okButton = new QPushButton(tr("OK"), &dialog);
    connect(okButton, &QPushButton::clicked, &dialog, &QDialog::accept);

    // Arrange widgets in a vertical layout
    QVBoxLayout *layout = new QVBoxLayout(&dialog);
    layout->addWidget(label);
    layout->addWidget(sizeSpinBox);
    layout->addWidget(okButton);

    // Show dialog and get result
    if (dialog.exec() == QDialog::Accepted) {
        int eraserSize = sizeSpinBox->value();

        // Set the eraser size as the current pen width and activate eraser mode
        QPen eraserPen;
        eraserPen.setColor(scene->backgroundBrush().color());  // Set eraser color to match the background
        eraserPen.setWidth(eraserSize);

        view->setPen(eraserPen);   // Set the pen in view as eraser
        view->setEraserMode(true); // Use the public setter to activate eraser mode
    }}

void GraphicsEditor::on_actionSaveScene_triggered()
{
  // Остановить все анимации или таймеры (зависит от того, как движутся объекты)
      stopMovingObjects();  // Эта функция должна остановить все анимации/таймеры
      // Открыть диалог для выбора пути сохранения
      QString filePath = QFileDialog::getSaveFileName(this, "Сохранить изображение", "", "Images (*.jpg *.png *.bmp)");
      if (filePath.isEmpty()) {
          return; // Если пользователь закрыл диалог, ничего не делаем
      }
      // Создаём изображение размером с область сцены
      QImage image(scene->sceneRect().size().toSize(), QImage::Format_ARGB32);
      image.fill(Qt::white); // Задаём белый фон
      // Рисуем содержимое сцены в изображение
      QPainter painter(&image);
      scene->render(&painter);
      painter.end();
      // Сохраняем изображение в файл
      if (!image.save(filePath)) {
          QMessageBox::warning(this, "Ошибка", "Не удалось сохранить изображение.");
      }
      // Возобновить все анимации или таймеры, если необходимо
      resumeMovingObjects();  // Эта функция возобновит анимации/таймеры
}
// Функция для остановки всех движущихся объектов
void GraphicsEditor::stopMovingObjects()
{
    for (int i = 0; i < movingStates.size(); ++i) {
        movingStates[i] = false;  // Устанавливаем состояние "остановлен"
    }
}
// Функция для возобновления движения всех движущихся объектов
void GraphicsEditor::resumeMovingObjects()
{
    for (int i = 0; i < movingStates.size(); ++i) {
        movingStates[i] = true;  // Устанавливаем состояние "движется"
    }
}
void GraphicsEditor::on_ActionAddImage_triggered()
{
  stopMovingObjects();
  // Открыть диалог для выбора изображения
      QString filePath = QFileDialog::getOpenFileName(this, "Выберите изображение", "", "Images (*.png *.jpg *.bmp *.jpeg)");
      if (filePath.isEmpty()) {
          return; // Если пользователь закрыл диалог, ничего не делаем
      }
      // Загружаем изображение из выбранного пути
      QPixmap pixmap(filePath);
      if (pixmap.isNull()) {
          // Если изображение не удалось загрузить, показываем сообщение об ошибке
          QMessageBox::warning(this, "Ошибка", "Не удалось загрузить изображение.");
          return;
      }
      // Создаем элемент для отображения изображения на сцене
      QGraphicsPixmapItem* item = new QGraphicsPixmapItem(pixmap);
      item->setData(0,"image");
      item->setFlag(QGraphicsItem::ItemIsMovable, true); // Фигуры можно перемещать
              item->setFlag(QGraphicsItem::ItemIsSelectable, true); // Фигуры можно выделять
              item->setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
      // Устанавливаем положение изображения на сцене (можно настроить по своему)
      item->setPos(scene->sceneRect().center() - pixmap.rect().center()); // Размещение по центру сцены
      // Добавляем элемент на сцену
      scene->addItem(item);
      resumeMovingObjects();

}
