# Music Is Math

This is an assignment project for CMU 15466/15666 course.



#### Author: Jialan Dong



#### Design:

Spheres are music tracks, and cubes are music made up with these sphere tracks. Each sphere/cube represents a non-negative integer in base-?. Your goal is to solve the music-math puzzle and figure out the question.

== equations satisfy both music composition and math arithmetic.

= equations satisfy only math arithmetic.



#### Screen Shot:

![Screen Shot](screenshot.png)



#### How To Play:

`Enter/Return` can let you enter/submit your answer;

`Mouse Cliks on a sphere/cube` can play the corresponding music the sphere/cube represents;

`Escape` can stop all sounds.

Use your brain to do the math part.

 

#### [Spoil Warning!] Solution:

Every level is a base-x arithmetic problem.

For better explanation, spheres are called tracks && cubes are called music.

##### level 1:

level 1 is base-2. Base-2 is the most intuitive non-decimal arithmetic. So this is used as the introduction. It also give player the idea about the "music equation".

`blue track = 10(2); green track = 11(3);`

green music is consist of blue track and green track, so it's `10+11=101(5)`.

`yellow track = 110 (6);` 

pink music is consist of blue track and yellow track. So it's `10+110=1000(8)`.



The answer is 1000.



##### level 2:

level 2 is base-3. Clearly, this level is clearly not base-2. So it expect players to realize this game may have other bases.

```
blue track + yellow track = 111 - 12;
blue track + yellow track = 22;
-> 111 - 12 = 22
-> base = 3

12(5) + yellow track = 102(11)
-> yellow track = 20(6)

blue track + 20(6) = 22(8)
->blue track = 2
```

The answer is 2.

 

##### level 3:

level3 is base-4. This level introduce non musical equations (but still math equations). It also requires player must recognize the tracks in the music to solve the problem.

```
1 + green track + yellow track = 22;
1 + 1 + 13 = pink music;

[listen] pink music is consist of green track and yellow track

-> 2 + 13 = green track + yellow track = 21;
-> base = 4;

green track + yellow track = 21(9);
green track * 2 = yellow track;
-> green track = 3, yellow track = 6(12);
```

 The answer is 12.



##### level 4:

This level is what this game is really for lol. Above are all tutorials.

```
first, let's name the first line's spheres from 'a' to 'g' (left to right).

a * b = 2;
Note that all tracks are non-negative integers;
-> a = 1 && b = 2 || a = 2 && b = 1;
-> a + b = 3;

d * e = 0;
-> d = 0 || e = 0;

10 - e = 3;
-> e != 0;
-> d = 0;

[listen] greyish-blue music = c + d + e + f;
[listen] pink music = e + g;
-> c + d + f = g;
given d = 0;
-> c + f = g;

[listen] brown music = a + b + c + f = 10;
given a + b = 3;
-> c + f = base - 3;
-> g = base - 3;

[listen] green music = a + b;
g - (a + b) + a * b = 1;
-> g - 3 + 2 = 1;
-> g = 2
-> base = 5
-> e = base - 3 = 2;

yellow music = a + b + c + d + e + f + g
= 3 + c + 0 + 2 + f + g
= 3 + (c + f) + 0 + 2 + 2
= 3 + 2 + 0 + 2 + 2
= 14(9)
```

The answer is 14.



This game was built with [NEST](NEST.md).
