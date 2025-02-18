# Fase 1 - Primitivas Gráficas

### Forma de execução
- **Generator** 

###### Na diretoria Fase1, de modo a gerar os ficheiros de build, executar os comandos:


```text
cd build ; cmake .. ; cmake --build .
```

Desta forma, os executáveis generator.exe e executable.exe são gerados. 

###### Gerar os ficheiros .3d que definem as primitivas desejadas a construir com o comando:

```text
./generator.exe plane 2 3 plane.3d
```

- **Engine** 

###### Executar engine com o comando:

```text
./engine.exe
```

