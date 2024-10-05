"CARRERA DE BUSES"
import os
import random
import time

GREN = "\033[32m"
END = "\033[0m"

def buses(n1, n2):
    output = []
    output.append(115 * "-")
    output.append((n1 * " ") + "_______________  " + ((100 - n1) * " ") + "|")
    output.append((n1 * " ") + "|__|__|__|__|__|___ " + ((97  - n1) * " ") + "|")
    output.append((n1 * " ") + "|      ANGEL      |)" + ((96  - n1) * " ") + " |")
    output.append((n1 * " ") + "|~~~@~~~~~~~~~@~~~|)" + ((95  - n1) * " ") + "  |")
    output.append(115 * "_")
    output.append((n2 * " ") + "_______________  " + ((100 - n2) * " ") + "|")
    output.append((n2 * " ") + "|__|__|__|__|__|___ " + ((97  - n2) * " ") + "|")
    output.append((n2 * " ") + "|      JAIRO      |)" + ((96  - n2) * " ") + " |")
    output.append((n2 * " ") + "|~~~@~~~~~~~~~@~~~|)" + ((95  - n2) * " ") + "  |")
    output.append(115 * "_")
    return "\n".join(output)

a = 0
b = 0
gano = None  # Inicializa 'gano' aquí

os.system("cls" if os.name == "nt" else "clear")
presentacion = """
        <<<<<<<<<<< carrera de buses >>>>>>>>>>
            ANGEL VS JAIRO """
print(presentacion)
time.sleep(3)

while a < 97 and b < 97:
    c = random.randint(1, 2)
    if c == 1:
        a += 1
    if c == 2:
        b += 1
    os.system("cls" if os.name == "nt" else "clear")
    print(buses(a, b))
    time.sleep(0.07)

if a >= 97:
    gano = "ANGEL"
if b >= 97:
    gano = "JAIRO"

print(f"{GREN}GANÓ LA CARRERA: {gano}{END}")
