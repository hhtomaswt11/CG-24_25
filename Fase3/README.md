# Fase 3 - Curvas, Superfícies Cúbicas e VBOs

### Forma de execução
- **Generator** 

###### Na diretoria Fase3, de modo a gerar os ficheiros de build, executar os comandos:


```text
mkdir build; cd build ; cmake .. ; cmake --build .
```

Desta forma, os executáveis generator e executable são gerados. 

###### Gerar os ficheiros .3d que definem as primitivas desejadas a construir com o comando:

```text
./generator box 2 3 box.3d
```

- **Engine** 

###### Executar engine com o comando:

```text
./engine ../test_files_phase_3/test_3_1.xml
```

###### Comando completo (deve ser executado na diretoria Fase2)
```text
mkdir build; cd build ; cmake .. ; cmake --build .  ; ./generator box 2 3 box.3d  ; ./engine ../test_files_phase_3/test_3_1.xml ; cd ..
```

