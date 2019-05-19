# CAD技术报告

url：https://github.com/dendenxu/CAD

为增加对gif文件的支持，没有转pdf，报告以markdown（转为html）形式呈现，编辑环境为typora。

[TOC]

## 1 成员信息

### 1.1 组员姓名

1. 徐震
2. 陈九润

### 1.2 组员学号

1. 3180105504
2. 3180105488

### 1.3 组员分工

1. 所有工作
2. 暂无



## 2 操作手册


### 2.1 ELLIPSE

- To draw, move or toggle an ellipse
  - Hit the `f3` key on your keyboard to tell the program that you selected the `ellipse` tool from the toolbox.
  - Press `left button` on your mouse within the graphic window area.
  - Move your mouse with `left button` down. Now you can see an ellipse appearing on your screen.
  - Release `left button`, the ellipse stays on your screen.
  - Hit `f5` to select the `hand` tool.
  - You can `left click` on an ellipse you've drawn to select it.(A `toggle box` should appear.)
  - Or `move` it around within the window. (A `toggle box` should appear.)
  - With your `toggle box ` on the screen, toggle the ellipse selected on the screen.
  - Want to close the window? Hit `Alt + f4`.

### 2.2 LINE

- To draw, move or toggle a line, just hit the `f1` key and do whatever you want as if it were an ellipse. I'll list what to do.
  - Hit the `f1` key on your keyboard to tell the program that you selected the `line` tool from the toolbox.
  - Press `left button` on your mouse within the graphic window area.
  - Move your mouse with `left button` down. Now you can see an line appearing on your screen.
  - Release `left button`, the line stays on your screen.
  - Hit `f5` to select the `hand` tool.
  - You can `left click` on an line you've drawn to select it.(A `toggle box` should appear.)
  - Or `move` it around within the window. (A `toggle box` should appear.)
  - With your `toggle box ` on the screen, toggle the line selected on the screen.
  - Want to close the window? Hit `Alt + f4`.

### 2.3 RECTANGLE

- To draw, move or toggle a rectangle, just hit the `f1` key and do whatever you want as if it were an line. I'll list what to do.
  - Hit the `f2` key on your keyboard to tell the program that you selected the `rectangle` tool from the toolbox.
  - Press `left button` on your mouse within the graphic window area.
  - Move your mouse with `left button` down. Now you can see an rectangle appearing on your screen.
  - Release `left button`, the rectangle stays on your screen.
  - Hit `f5` to select the `hand` tool.
  - You can `left click` on an rectangle you've drawn to select it.(A `toggle box` should appear.)
  - Or `move` it around within the window. (A `toggle box` should appear.)
  - With your `toggle box ` on the screen, toggle the rectangle selected on the screen.
  - Want to close the window? Hit `Alt + f4`.

### 2.4 TEXTFRAME

- To draw or move a text frame along with its text, just hit the `f4` key and do whatever you want as if it were an ellipse. I'll list what to do.
  - Hit the `f4` key on your keyboard to tell the program that you selected the `textframe` tool from the toolbox.
  - Press `left button` on your mouse within the graphic window area.
  - Move your mouse with `left button` down. Now you can see an line appearing on your screen.
  - Release `left button`, the line stays on your screen.
  - Then you're supposed to see twinkling cursor at the up-left corner of the `textframe`.
  - Now type what you want to see on the screen inside the `textframe`. Note that `Backspace`, `Delete`, `Left`, `Right`, `Enter` keys are supported while editing your text.
  - The text-frame supports line buffer, which means once you've hit the `enter` key, the text of the previous line cannot be selected from now on.(Unless you're already at the bottom of the text frame, where `enter` don't submit the line being edited.)
  - Please avoid Chinese word. If you enter Chinese, scrambled code may appear, or `Song-Ti` Character may appear but cursor behaves badly. In all, don't try.
  - **REMEMBER TO HIT `ESC` WHEN YOU'RE DONE WITH THIS TEXT-FRAME**. Or... bad things may happen. Or maybe nothing happens.
  - Hit `f5` to select the `hand` tool.
  - You can `left click` on the **frame** of the text you've typed to select it.(A `toggle box` should appear. Bigger than that of a rectangle or ellipse or line.)
  - Or `move` it around within the window. (A `toggle box` should appear. Bigger than that of a rectangle or ellipse or line.)
  - Want to close the window? Hit `Alt + f4`.



## 3 基本实现思路

### 3.1 椭圆

#### 3.1.1 add

在全局物件数组中添加新椭圆结构，由鼠标移动触发，四个参数（x，y，rx，ry）由鼠标原坐标（omx，omy）计算得到，添加过程中调用toggle函数，对新加入的椭圆大小进行改变，实现添加过程中直接用鼠标调整大小的功能。

#### 3.1.2 move

根据椭圆的四个参数，记（ix，iy）为椭圆上的点，由$ix=x+rx*cos\theta\ \ iy = y+sin\theta$得到ix，iy，检测鼠标左键点击位置是否在该点的某距离范围内（通过循环以及足够小的$\mathrm{d}\theta$遍历整个椭圆）。若在，则将该物件选中，调整全局结构变量elemt的值。根据鼠标的移动，调用add函数（另一种思路是直接修改该物件的四个参数，在文本框的移动中采用了该种实现方式）。

#### 3.1.3 toggle

在选中物件的条件下（屏幕会有八个变换点显示），检测鼠标点击位置是否在变换点之内，若是，根据鼠标的移动重新调用add函数（另一种思路是直接修改该物件的四个参数）。

#### 3.1.4 delete

根据elemt.index在全局元素数组中删除选中的元素。并释放相应的手动内存分配空间。

### 3.2 线段

#### 3.2.1 add toggle delete

基本同椭圆。

#### 3.2.2 move

根据线段的四个参数，记（ix，iy）为直线段上的点，由

```c
dt = (fabs(dx) > fabs(dy) ? InchesX(1) : InchesY(1)) / (fabs(dx) > fabs(dy) ? fabs(dx) : fabs(dy));//目的是保证dt大于零，使最终的ix iy在线段上而不是线段向某一个方向的射线上。
ix = x + dx * t; 
iy = y + dy * t;
```

得到直线上的点，其余思路与椭圆中基本相同。

### 3.3 矩形

#### 3.3.1 add toggle delete

基本同上。

#### 3.3.2 move

将线段的检查思路拓展为矩形的四条边。

### 3.4 文本框

#### 3.4.1 move delete

基本同矩形。

#### 3.4.2 add

添加元素并用鼠标控制大小的过程与矩形类似。但拖动完成松开鼠标后可观察到屏幕上光标的闪烁，即表明进入text模式，在此模式下`charEventProcess`以及`keyboardEventProcess`都会走不同的运行路径以实现字符在屏幕上的输出。

字符输出过程中，基本思路与提交的作业中TEXT的有相似之处，但也有不小的不同。TEXT采取的行缓冲，是实际上的行缓冲，即敲回车后上一行的屏幕内容会被完全确定，不再进行任何修改或重新输出。而本项目中采取的是形式上的行缓冲，即敲回车后上一行的内容仍然会有清空屏幕并重新输出的过程（目的是保证输出过程中不会掩盖背后的重叠部分）。其他字符操作，如左右键，backspace，delete与TEXT作业（光标发生移动过程中保证光标是出现的）中类似。

文本编辑完成后应敲击Esc以进行文本输入的收尾工作（调整全局变量内容等），否则会发生难以描述的事情。

### 3.5 DrawEverything

该函数会遍历元素数组（有人考虑用链表实现，目的是元素可以无限多，其实这是不现实的，在元素个数超过20个时，若仍要保持最大编辑性，机器会产生明显卡顿），重新输出所有内容。

与BitBlt函数配合使用，并用InvalidateRect设置刷新区域，进行屏幕的刷新。

刷新思路：

1. 用BitBlt函数清空整个屏幕。
2. 用InvalidateRect设置整个屏幕为刷新区域以防止拖尾，并将最后一个参数设为FALSE以防止闪烁。
3. 用DrawEverything输出所有应该被输出的元素（包括某个元素四周的调整框以及调整点）。

对屏幕进行刷新的情况：

- 鼠标左键按下
- 鼠标左键抬起
- 鼠标移动
- 文本编辑状态下
  - 键入字符（包括回车）
  - 进行操作：删除（backspace/delete等）
  - 保存字符：敲击Esc



### 3.6 矢量

本项目中所有元素（包括字符），都以矢量形式储存在内存中，而不是进行栅格化后储存，所以项目思路更接近AI而不是PS。前期考虑过对内容直接进行渲染后栅格化储存，目的是减少CPU压力，保证程序运行平滑。在后期测试中，发现i7-8550U在睿频状态下可支持二十个以上普通元素（无文本框）的实时渲染（如果有文本框，则为内容不多的文本框十个左右），可基本满足使用要求。而且以矢量方式能够良好保证各个元素的图层性，不同元素间的遮盖关系可以完美实现。椭圆是中空的就是中空的，不会对后方的矩形产生遮盖，甚至文字也不会对后方内容产生遮盖。

矢量方式避免了对屏幕内容的直接操作，所有操作都是抽象的，所以不存在移动元素到边缘再移动回来会被切割的问题。



### 3.7 鼠标

项目中，对于元素的添加，调整，移动，都是通过鼠标移动来控制的。有以下几个特点：

- 代码复杂，变数多，需更细心的实现
- 可操控性极强，能保证大小、位置的（基本）连续变化
- 符合PS/AI使用者的操作习惯，虽然简单，但实际实现并不容易



## 4 特点

### 4.1 对roberts库内容进行了改进

下面是主要diff结果（项目中的graphics.c与钱徽老师网站上的graphics.c）

![TIM图片20190518112216](.\TIM图片20190518112216.png)

![TIM图片20190518112259](.\TIM图片20190518112259.png)

![TIM图片20190518112335](.\TIM图片20190518112335.png)

![TIM图片20190518112338](.\TIM图片20190518112338.png)

![TIM图片20190518112341](.\TIM图片20190518112341.jpg)

![TIM图片20190518112344](.\TIM图片20190518112344.png)

![TIM图片20190518112347](.\TIM图片20190518112347.png)

![TIM图片20190518112350](.\TIM图片20190518112350.png)

![TIM图片20190518112354](.\TIM图片20190518112354.png)

![TIM图片20190518112358](.\TIM图片20190518112358.png)

#### 4.1.1 InvalidateRect函数

下面贴msc的手册：

![TIM图片20190518113418](.\TIM图片20190518113418.png)

该函数的不当使用会导致两个问题：

1. 闪烁

   ![1.](.\1.gif)

2. 拖尾（toggle和move没有拖尾：在测试时给它俩设置了足够大的刷新区域）

   ![2](.\2.gif)

解决方式：

1. 设置最后一个参数为FALSE而不是TRUE
2. 设置足够大的刷新区域

#### 4.1.2 BitBlt函数

下面贴msc的手册：
![TIM图片20190518120937](.\TIM图片20190518120937.png)

![TIM图片20190518120941](.\TIM图片20190518120941.png)

![TIM图片20190518120944](.\TIM图片20190518120944.png)

虽然函数本质是拷贝屏幕像素内容，但本项目中采用该函数进行屏幕清空操作（最后一个参数使用WHITENESS）。

### 4.2 文本框支持

对文本框支持以下高级内容：

- 输入英文文本（包括monospace字体以及non-monospace字体），并在屏幕上正确显示它们，并同时保证其他重叠图层的正确显示
- 支持光标的闪烁，左右移动（并保证移动过程中光标始终高亮），移动后可在光标位置进行操作（输入字符，删除等）
- 支持退格键，delete键（同时保证内容的正确显示）
- 根据文本框大小限制输入的行数和每一行的字符数

### 4.3 基本功能演示

（实际使用时其实更为流畅[斜眼笑]）

![3](.\3.gif)

### 4.4 想说的

项目没有过分关照界面的美观性等问题，例如漂亮的工具栏按钮，漂亮的颜色，对颜色改变的支持，对字体改变的支持，对画笔大小以及字号改变的支持，以及图层相关功能等。

因为作者认为在解决了底层实现问题后那些功能可以很方便地添加。

现今若要改变文字大小等，应在宏处改变LAMBDA的展开值。

若要改变字体，请在宏处改变FONT的展开值。

若要改变颜色，请直接改变全局变量中PENCOLOR的值（设为全局变量也是为了日后的拓展性）。

相对于一个华丽，但是问题多多（或者以治标不治本方式掩盖问题）的项目（例如闪烁，刷新区域拖尾，对文本框的支持，多图层的实现等），作者更希望打好基础。



## 5 环境

在Visual Studio 2017 64 debug环境下开发

测试在VS x86环境下、Devcpp x64/x86（请声明-std=c99）环境下可编译运行（需对roberts库中的RaiseException函数进行调整）

项目基于windows图形库，请不要尝试在非windows环境下编译运行。