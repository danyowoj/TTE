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
    createMovingObject_1();

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
}

void GraphicsEditor::moveObject()
{
    int wallThickness = 10;

    for (int i = 0; i < movingItemGroups.size(); ++i) {
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

//                bool collisionDetected = false;
//                QList<QGraphicsItem *> itemsAtNewPos = scene->items(QRectF(newPos, boundingRect.size()));
//                for (QGraphicsItem *otherItem : itemsAtNewPos) {
//                    if (otherItem != itemGroup  && otherItem->data(0) != "user") {
//                        QRectF otherBoundingRect = otherItem->boundingRect().translated(otherItem->pos());

//                        if (right > otherBoundingRect.left() && left < otherBoundingRect.right()) {
//                            collisionDetected = true;
//                                            break;
//                        }

//                        if (bottom > otherBoundingRect.top() && top < otherBoundingRect.bottom()) {
//                            collisionDetected = true;
//                                            break;
//                        }
//                    }
//                }

//                if (collisionDetected) {

//                            velocity.setX(-velocity.x()); // Изменяем направление по оси X при столкновении
//                            velocity.setY(-velocity.y()); // Изменяем направление по оси Y при столкновении
//                            collisionSound.play();  // Звук столкновения
//                        }
                // Обработка столкновений с другими объектами
                bool collisionDetected = false;
                QList<QGraphicsItem *> itemsAtNewPos = scene->items(QRectF(newPos, boundingRect.size()));

                for (QGraphicsItem *otherItem : itemsAtNewPos) {
                    if (otherItem != itemGroup && otherItem->data(0) != "user") {
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
