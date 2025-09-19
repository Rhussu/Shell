# 💻 Shell - Tarea 1 de Sistemas Operativos

El informe de la tarea se encuentra en el repositorio como [*Informe.pdf*](), pero también está disponible en este README en la sección de [📄 Informe](#-informe).

> ⚠ **Aviso:** Todavía no está disponible *Informe.pdf*.

---

## 🚀 Getting Started

Nuestra **Shell** funciona únicamente en **Linux x86_64**.  
Actualmente hay **3 formas** de usarla:


### 1️⃣ Opción 1: Usando la release de GitHub (sin clonar el repositorio)

Puedes descargar la **última versión** de la shell directamente desde la sección de releases:

[![Release](https://img.shields.io/github/v/release/Rhussu/Shell?style=flat-square)](https://github.com/Rhussu/Shell/releases/latest)

**Instrucciones para ejecutar**:  

1. Descarga el archivo desde GitHub.  
2. Abre una terminal y navega a la carpeta donde se descargó.  
3. Ejecútalo con:

```bash
./shell-<version>-x86_64-linux
```
> ⚠ Asegurate de cambiar `<version>` por la versión que descargaste.




### 2️⃣ Opción 2: Con `make`

Si tienes `make` instalado en tu dispositivo, clona el repositorio y desde la raíz de este puedes ejecutar los comandos indicados en tu Makefile.  
Esto instalará las dependencias necesarias, compilará la shell y la ejecutará automáticamente.

```bash
make install_dependencies shell run_shell
```

### 3️⃣ Opción 3: Sin `make`

Instala las dependencias del proyecto manualmente con:

```bash
sudo apt update
sudo apt install -y build-essential make
```

> 💡 **Tip:** Si en este punto ya instalaste `make`, puedes continuar fácilmente siguiendo la [2️⃣ Opción 2: Con `Makefile`](#2️⃣-opción-2-con-makefile) para compilar y ejecutar la shell automáticamente.

Luego, clona el repositorio y en la raíz de este ejecuta:
```bash
gcc -Wall -Iinclude src/main.c src/shell.c -o build/shell
./build/shell
```


## 📄 Informe

> ⚠ **Aviso:** Todavía no está disponible el informe.

