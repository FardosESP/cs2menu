import os
import subprocess
import sys
import shutil
import urllib.request
import zipfile

# Configuración del proyecto
PROJECT_NAME = "cs2menu"
REPO_ROOT = os.path.dirname(os.path.abspath(__file__))
LIB_DIR = os.path.join(REPO_ROOT, "lib")
BUILD_DIR = os.path.join(REPO_ROOT, "build")
LOG_FILE = os.path.join(REPO_ROOT, "build_log.txt")

def log(message):
    print(message)
    with open(LOG_FILE, "a", encoding="utf-8") as f:
        f.write(message + "\n")

def run_command(command, cwd=None):
    log(f"Ejecutando: {' '.join(command)}")
    try:
        process = subprocess.Popen(
            command,
            cwd=cwd,
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            shell=True,
            text=True,
            encoding="utf-8"
        )
        for line in process.stdout:
            log(line.strip())
        process.wait()
        return process.returncode
    except Exception as e:
        log(f"Error al ejecutar comando: {e}")
        return 1

def download_dependency(name, url, target_dir):
    if os.path.exists(target_dir) and os.listdir(target_dir):
        log(f"[INFO] {name} ya existe en {target_dir}. Saltando descarga.")
        return True

    log(f"[INFO] Descargando {name} desde {url}...")
    os.makedirs(target_dir, exist_ok=True)
    zip_path = os.path.join(LIB_DIR, f"{name}.zip")
    
    try:
        urllib.request.urlretrieve(url, zip_path)
        with zipfile.ZipFile(zip_path, 'r') as zip_ref:
            # Extraer en una carpeta temporal
            temp_extract = os.path.join(LIB_DIR, f"temp_{name}")
            zip_ref.extractall(temp_extract)
            
            # Mover el contenido de la carpeta interna al target_dir
            inner_folder = os.path.join(temp_extract, os.listdir(temp_extract)[0])
            for item in os.listdir(inner_folder):
                shutil.move(os.path.join(inner_folder, item), os.path.join(target_dir, item))
            
            shutil.rmtree(temp_extract)
        os.remove(zip_path)
        log(f"[OK] {name} instalado correctamente.")
        return True
    except Exception as e:
        log(f"[ERROR] Fallo al descargar {name}: {e}")
        return False

def main():
    if os.path.exists(LOG_FILE):
        os.remove(LOG_FILE)
    
    log("=== CS2MENU AUTOCONFIGURADOR MARZO 2026 ===")
    log(f"Directorio raíz: {REPO_ROOT}")

    # 1. Verificar y descargar dependencias
    os.makedirs(LIB_DIR, exist_ok=True)
    
    imgui_url = "https://github.com/ocornut/imgui/archive/refs/heads/master.zip"
    minhook_url = "https://github.com/TsudaKageyu/minhook/archive/refs/heads/master.zip"
    
    if not download_dependency("imgui", imgui_url, os.path.join(LIB_DIR, "imgui")):
        log("[ERROR] No se pudo preparar ImGui. Abortando.")
        return

    if not download_dependency("minhook", minhook_url, os.path.join(LIB_DIR, "minhook")):
        log("[ERROR] No se pudo preparar MinHook. Abortando.")
        return

    # 2. Configurar con CMake
    if os.path.exists(BUILD_DIR):
        log("[INFO] Limpiando carpeta build anterior...")
        shutil.rmtree(BUILD_DIR)
    os.makedirs(BUILD_DIR)

    log("[INFO] Configurando proyecto con CMake...")
    cmake_cmd = ["cmake", "..", "-G", "Visual Studio 17 2022", "-A", "x64"]
    if run_command(cmake_cmd, cwd=BUILD_DIR) != 0:
        log("[ERROR] Fallo en la configuración de CMake. Revisa build_log.txt")
        return

    # 3. Compilar
    log("[INFO] Compilando DLL en modo Release...")
    build_cmd = ["cmake", "--build", ".", "--config", "Release"]
    if run_command(build_cmd, cwd=BUILD_DIR) != 0:
        log("[ERROR] Fallo en la compilación. Revisa build_log.txt")
        return

    dll_path = os.path.join(BUILD_DIR, "Release", f"{PROJECT_NAME}.dll")
    if os.path.exists(dll_path):
        log("="*40)
        log(f"[EXITO] DLL generada en: {dll_path}")
        log("="*40)
        log("Ahora puedes inyectar cs2menu.dll en CS2 (modo -insecure).")
    else:
        log("[ERROR] La compilación terminó pero no se encontró la DLL.")

if __name__ == "__main__":
    main()
