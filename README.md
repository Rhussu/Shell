# 💻 BCRZ Shell - Tarea 1 de Sistemas Operativos

El informe de la tarea se encuentra en el repositorio como [*Informe.pdf*](https://github.com/Rhussu/Shell/blob/main/Informe.pdf)

---

## 🚀 Getting Started

Nuestra **BCRZ shell** funciona únicamente en **Linux x86_64**.  
Actualmente hay **3 formas** de usarla:


### 1️⃣ Opción 1: Usando la release de GitHub (sin clonar el repositorio)

> ⚠️ **Advertencia:**  
> Esta opción requiere **Linux con glibc >= 2.35**, si no cumples con eso te recomendamos **no utilizar esta opción**.

Puedes descargar la **última versión** de la shell directamente desde la sección de releases:

[![Release](https://img.shields.io/github/v/release/Rhussu/Shell?style=flat-square)](https://github.com/Rhussu/Shell/releases/latest)

**Instrucciones para ejecutar**:  

1. Descarga el archivo desde GitHub.
2. Abre una terminal y navega a la carpeta donde se descargó.  
> 💡 **Tip:** Tambien puedes ejecutar el siguiente comando para descargarlo desde terminal en tu carpeta actual:
>```bash
>  curl -L -o BCRZ-x86_64-linux https://github.com/Rhussu/Shell/releases/latest/download/BCRZ-x86_64-linux
>```
3. Dale permisos de ejecución y ejecútalo con:

```bash
chmod +x BCRZ-x86_64-linux
./BCRZ-x86_64-linux
```




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

> 💡 **Tip:** Si en este punto ya instalaste `make`, puedes continuar fácilmente siguiendo la [2️⃣ Opción 2: Con `Makefile`](#2%EF%B8%8F⃣-opción-2-con-make) para compilar y ejecutar la shell automáticamente.

Luego, clona el repositorio y en la raíz de este ejecuta:
```bash
gcc -Wall -Iinclude src/main.c src/shell.c -o build/shell
./build/shell
```

