# CAD



## Property

- Author: xudenden
- Starting date: 28-4-2019
- Dependency: E.Roberts pc-borland graphics library(qianhui modified)(augmentation inside the library is done)



## Funtion

1. Draw a line, rectangle or ellipse on the screen.
2. Move them like in PS.
3. Toggle them like in PS.
4. Text frame



## Update

1. Text done
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
     - You can `left click` on the frame of the text you've typed to select it.(A `toggle box` should appear. Bigger than that of a rectangle or ellipse or line.)
     - Or `move` it around within the window. (A `toggle box` should appear. Bigger than that of a rectangle or ellipse or line.)
     - Want to close the window? Hit `Alt + f4`.
2. Bad problems when deleting the last object fixed.



## Update

1. Line done

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
   
2. Rectangle done

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
3. It suddenly occurs to me that those three things were using the same structure.

   Is it possible to toggle the codes to make it shorter?

4. `Delete` key is also supported now.



## Update

1. CAD rebase finished
   - Remove `frame`
   - Remove `slect`
   - Perfect `Delete()`funtion
   - Add `Toolbox`



## Update

1. Now you can draw an ellipse(or many ellipses) like in any other graphic processing software.
2. Movement also supported for ellipse.
3. Toggling is supported as well.
4. How to try those features?
   - Hit the `f3` key on your keyboard to tell the program that you selected the `ellipse` tool from the toolbox.
   - Press `left button` on your mouse within the graphic window area.
   - Move your mouse with `left button` down. Now you can see an ellipse appearing on your screen.
   - Release `left button`, the ellipse stays on your screen.
   - Hit `f5` to select the `hand` tool.
   - You can `left click` on an ellipse you've drawn to select it.(A `toggle box` should appear.)
   - Or `move` it around within the window. (A `toggle box` should appear.)
   - With your `toggle box ` on the screen, toggle the ellipse selected on the screen.
   - Want to close the window? Hit `Alt + f4`.



## My Thoughts

You think it works smoothly, don't you?

Hhhhh.

The actual source **IS COMPLETELY A MESS**.

### A MESS!!

Nothing more to say.

Feeling dead already.

### Well...

After two hours, the logic is becoming clearer.

I've noticed that you should never just change things. You should have a plan.

Coding a project is like building a kingdom. hhhh.

