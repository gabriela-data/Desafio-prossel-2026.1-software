#!/bin/bash

echo "Configurando o projeto com correções para GTest e FreeType..."

# Usamos -B para definir a pasta de build e -S para a pasta de código-fonte (.)
# Os caminhos do FreeType são ajustados para apontar para dentro de build/_deps
cmake -B build -S . \
    -DBUILD_TESTING=OFF \
    -DFREETYPE_INCLUDE_DIR_ft2build=$(pwd)/build/_deps/sfml-src/extlibs/headers/freetype2 \
    -DFREETYPE_INCLUDE_DIR_freetype2=$(pwd)/build/_deps/sfml-src/extlibs/headers/freetype2 \
    -DFREETYPE_LIBRARY=/usr/lib/x86_64-linux-gnu/libfreetype.so.6

if [ $? -ne 0 ]; then
    echo "Erro na configuração do CMake."
    exit 1
fi

echo "Iniciando a compilação..."
cmake --build build -j$(nproc)

if [ $? -eq 0 ]; then
    echo "------------------------------------------------"
    echo "Compilação concluída com sucesso!"
    echo "Para rodar o simulador: ./build/SimuladorFutebol"
    echo "------------------------------------------------"
else
    echo "Erro durante a compilação."
    exit 1
fi
