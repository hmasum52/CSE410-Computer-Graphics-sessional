import turtle
from PIL import Image
import os


def set_up_screen(width: int = 600, height: int = 400):
    screen = turtle.Screen()
    screen.setup(width, height)
    screen.title('Fractal Tree')
    screen.bgcolor('black')
    # screen.screensize(3*width, 3*height)
    screen.delay(0)


# number of iterations
n_gen = 1
forward_distance = 20
screen_width = 1200
screen_height = 800


# create an instance of turtle
turbo = turtle.Turtle()
turbo.pensize(3)
turbo.speed(0)
turbo.color("white")

stack = []

variables = []
constants = []
start = ''
angle = 0
rules = []
rule_map = {}


def produce(axiom, rule_map):
    return ''.join([rule_map[chr] if chr in rule_map else chr for chr in axiom])

def get_production(gens, axiom, rule_map):
    for _ in range(gens):
        axiom = produce(axiom, rule_map)
    return axiom

def increase_gen():
    global n_gen
    n_gen += 1
    turtle.clear()
    turtle.write(f'Generation: {n_gen}', font=('Arial', 16, 'normal'))
    draw_fractal()

def decrease_gen():
    global n_gen
    if n_gen > 1:
        n_gen -= 1
        turtle.clear()
        turtle.write(f'Generation: {n_gen}', font=('Arial', 16, 'normal'))
        draw_fractal()

def draw_fractal():
    turbo.pensize(3)
    turbo.penup()
    turbo.goto(-100,-150);
    turbo.pendown()
    turbo.clear()

    rule_body= get_production(n_gen, start, rule_map)
    print(rule_body)
    for symbol in rule_body:
        if symbol in variables:
            turbo.forward(forward_distance)
        elif symbol == '+':
            turbo.left(angle)
        elif symbol == '-':
            turbo.right(angle)
        elif symbol == '[':
            stack.append((turbo.heading(), turbo.pos()))
        elif symbol == ']':
            heading, position = stack.pop()
            turbo.setheading(heading)
            turbo.penup()
            turbo.goto(position)
            turbo.pendown()

def save_fractal():
  canvas = turtle.getcanvas()
  psFilename = "foo.ps"
  canvas.postscript(file=psFilename)
  psimage = Image.open(psFilename)
  psimage.save("bar.png") # or any other image format that PIL supports.
  os.remove(psFilename) # optional


turtle.onkeypress(increase_gen, '+')
turtle.onkeypress(decrease_gen, '-')
turtle.onkeypress(save_fractal, '0')
turtle.listen()


if __name__ == '__main__':
    filename = 'input1.txt'
    # filename = input("Enter the filename: ")

    with open(filename, 'r') as file:
        lines = file.readlines()

    variables = lines[0].strip().split(': ')[1].split(',')
    constants = lines[1].strip().split(': ')[1].split(',')
    start = lines[2].strip().split(': ')[1]
    angle = float(lines[3].strip().split(': ')[1])
    rules = [rule.strip() for rule in lines[4].strip().split(': ')[1].split(',')]
    rule_map = {rule.split('->')[0]: rule.split('->')[1] for rule in rules}


    # print inputs
    print(variables)
    print(constants)
    print(start)
    print(angle)
    print(rules)
    print(rule_map)

    set_up_screen(screen_width, screen_height)

    turtle.pencolor('white')
    turtle.goto(-screen_width//3 + 20, -screen_height/3 + 20)
    turtle.clear() # clear the screen
    turtle.write(f'Generation: {n_gen}', font=('Arial', 16, 'normal'))

    turbo.setheading(90)
    turbo.pensize(3)

    draw_fractal()

    turtle.done()

    