# Alocador de professores
Trabalho da disciplina de teoria e aplicação de grafos da Universidade de Brasília feito por Ruan Petrus.

## Enunciado do problema
Considere para efeito deste projeto que uma determinada unidade da federação fez um concurso e foram aprovados cem (100) novos professores para escolas públicas. Cada professor aprovado possui uma (1), duas (2), ou até (3) habilitações de conteúdos em que pode atuar. Cinquenta (50) escolas se habilitaram a receber novos professores, sendo que algumas poderão receber no máximo um (1) professor, e outras no máximo dois (2) professores. As escolas podem indicar preferências de professores indicando se 3, 2 ou 1 habilitação os candidatos devem ter. Por sua vez, cada professor pode escolher uma ordem de até quatro (4) escolas onde gostaria de atuar. Neste projeto você deve implementar um algoritmo que realize um emparelhamento estável máximo, devendo também incluir pelo menos 1 professor para cada escola, e indicar quantos professores poderão ser alocados estavelmente. As soluções dadas em (Abraham, Irving & Manlove, 2007) são úteis e qualquer uma pode ser implementada com variações pertinentes.  
Um arquivo entradaProj2TAG.txt com as indicações de código do professor, habilitações e preferências de escolas, bem como das escolas com suas preferências em termos de habilitações dos professores é fornecido como entrada. Uma versão pública do artigo de (Abraham, Irving & Manlove, 2007) é fornecida para leitura.

## Como rodar o programa
Compile e depois rode o programa nos dados de entrada:
```sh
make
./allocation_optmizer entradaProj2TAG.txt
```
Ou rode direto com o comando:
```sh
make run
```

## Referencias
- Aulas do curso de Teoria e Aplicação de grafos pela Universidade de Brasília.
- https://www.digitalocean.com/community/tutorials/queue-in-c
- https://www.youtube.com/watch?v=nvRkFi8rbOM
- https://www.youtube.com/watch?v=tq3zPnrQIpU
