# Asteroids
A classic game implemented in an embedded programming environment.

[![YouTube video](https://img.youtube.com/vi/dR12arl7Y-8/0.jpg)](https://www.youtube.com/watch?v=dR12arl7Y-8)

## Overview
This is the final project for my ECEN 330: Introduction to Embedded Systems Programming class at Brigham Young University. I teamed up with one other individual (many thanks to Alex McBride) to create a custom project implementing various features of the board. In our case we, we chose to recreate a stripped down version of the classic arcade game, Asteroids. Our implementation used both the GPIO pins--which the push button switches were connected to--and the output display.

## Key Technologies

| Programming Language | Hardware                                 | Environmnet                           |
| -------------------- | ---------------------------------------- | ------------------------------------- |
| - C                  | - ZYBO Board by Digilent (via emulation) | - Manjaro Linux running ZYBO emulator |

## Features / Capabilities
- Full game loop including a title screen, general gameplay, and game over screen
- Player-controllable spaceship that can accelerate, rotate, and fire projectiles
- Asteroids which fragment upon coliding with a player-projectile
- Life- and score-tracking system

## What I Learned
- I learned how to install a Linux operating system in a dual-boot configuration as a powerful way to run the instructor-required software.
- I learned to code collaboratively by using Git for the first time.
- Embedded environments have certain limitations that we was able to overcome by innovating our solution.
- Good code involves so much more than merely working as anticipated.

## Notes
There are still a healthy amount of bugs in this implementation, but overall it was a huge success!
