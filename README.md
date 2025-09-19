# 💻 Shell - Tarea 1 de Sistemas Operativos

El informe de la tarea se encuentra en el repositorio como [*Informe.pdf*](), pero también está disponible en este README en la sección de [📄 Informe](#-informe).

> ⚠ **Aviso:** Todavía no está disponible *Informe.pdf*.

---

## 🚀 Getting Started

Nuestra **Shell** funciona únicamente en **Linux x86_64**.  
Actualmente hay **3 formas** de usarla:


### 1️⃣ Opción 1: Usando la release de GitHub

En la sección de releases de este repositorio hay un ejecutable listo para usar:

[![Release](https://img.shields.io/github/v/release/Rhussu/Shell?style=flat-square)](https://github.com/Rhussu/Shell/releases/latest)

Haz clic en el badge para descargar la última versión de la shell.



### 2️⃣ Opción 2: Con `Makefile`

Si tienes `make` instalado en tu dispositivo, desde la raíz del repositorio puedes ejecutar los comandos indicados en tu Makefile.  
Esto instalará las dependencias necesarias, compilará la shell y la ejecutará automáticamente.

```bash
make install_dependencies shell run_shell
```

### 3️⃣ Opción 3: Sin `Makefile`

Si no tienes `make` instalado, primero instala las herramientas necesarias en tu sistema y luego compila manualmente.  
Con esto, la shell debería funcionar correctamente.

```bash
sudo apt update
sudo apt install -y build-essential make
gcc -Wall -Iinclude src/main.c src/shell.c -o build/shell
./build/shell
```

> 💡 **Tip:** Si en este punto ya instalaste `make`, puedes continuar fácilmente siguiendo la [2️⃣ Opción 2: Con `Makefile`](#2️⃣-opción-2-con-makefile) para compilar y ejecutar la shell automáticamente.

Compilación y ejecución manual:
```bash
gcc -Wall -Iinclude src/main.c src/shell.c -o build/shell
./build/shell
```


## 📄 Informe

> ⚠ **Aviso:** Todavía no está disponible el informe.
