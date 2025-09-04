# darcyInterFoam

## Application
`darcyInterFoam`

## Group
`grpMultiphaseSolvers`

**Baseado em:**
- `interIsoFoam` — solver derivado do **interFoam** para dois fluidos incompressíveis e imiscíveis, usando o método isoAdvector (VOF) para captura de interface.

## Descrição

`darcyInterFoam` é um solver multifásico incompressível (VOF) para simular escoamentos imiscíveis com resistência de Darcy adicionada às equações de movimento.  
Ele inclui os efeitos de resistência linear (inércia negligenciada) em regiões porosas, baseado no `interIsoFoam`, implementando uma fonte de Darcy na equação de quantidade de movimento.

## Visão geral

Este repositório contém os seguintes arquivos principais:

```
Make
DarcyInterFoam.C
UEqn.H
pEqn.H
rhofs.H
```

> O arquivo **DarcyInterFoam.C** contém o controle de tempo, estrutura do solver, leitura de campos e chamada das equações principais.  
> **UEqn.H** define a equação de quantidade de movimento incluindo o termo de resistência de Darcy.  
> **pEqn.H** monta o acoplamento pressão/velocidade via PIMPLE, atualizando pressão e corrigindo o fluxo.  
> **rhofs.H** faz a interpolação das densidades das fases ($\rho_1$, $\rho_2$).  
> Todos juntos integram o solver para simulação de escoamento multifásico em meios porosos.


**Equação da continuidade (incompressível):**

$$
\nabla \cdot \mathbf{U} = 0
$$

**Equação da quantidade de movimento (com Darcy):**

$$
\frac{\partial (\rho \mathbf{U})}{\partial t} + \nabla \cdot \left( \rho \mathbf{U} \mathbf{U} \right) = - \nabla p_{rgh} + \nabla \cdot \boldsymbol{\tau} - \frac{1}{K}\mathbf{U} + \mathbf{f}_\sigma
$$

## 🔹 Notação

| Símbolo            | Significado                                                   |
|--------------------|---------------------------------------------------------------|
| $\mathbf{U}$       | Vetor velocidade                                              |
| $p$                | Pressão modificada ($p_{rgh} = p - \rho g h$)                 |
| $\mu$              | Viscosidade dinâmica                                          |
| $K$                | Permeabilidade intrínseca do meio poroso (m²)                 |
| $\rho$             | Densidade local (interpolada pelo VOF)                        |
| $\mathbf{f}_\sigma$| Força de tensão superficial entre as fases                    |

---


## Requisitos

- OpenFOAM (versão compatível com a base do código, por ex. v2206, v2306, v2406).
- Ambiente configurado (executar `source $FOAM_ETC/bashrc` ou similar).

---

## Como compilar

```bash
cd $FOAM_USER_APPBIN/..path../darcyInterFoam
wclean && wmake

## Como rodar (Alterações dem um case padrão )

Adicione o arquivo **`porousProperties`** dentro da pasta `constant` do case. Esse arquivo descreve a região porosa usada pelo solver.

---

### Exemplo de arquivo (`case/constant/porosityProperties`)
```text
/*--------------------------------*- C++ -*----------------------------------*\
| =========                 |                                                 |
| \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox           |
|  \\    /   O peration     | Version:  v2406                                 |
|   \\  /    A nd           | Website:  www.openfoam.com                      |
|    \\/     M anipulation  |                                                 |
\*---------------------------------------------------------------------------*/
FoamFile
{
    version     2.0;
    format      ascii;
    class       dictionary;
    location    "constant";
    object      porousProperties;
}
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

porousRegion
{
    K   7.4617e-11;
}

// ************************************************************************* //


---
