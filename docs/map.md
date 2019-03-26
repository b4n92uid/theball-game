# Configuration du niveau

## Champs pour la carte

### `script`

Chemein relatife au fichier de la carte qui définie le script LUA
qui est charger de traiter la partie logique de la carte

### `screenshot`

Chemein relatife au fichier de la carte qui définie l'image d'aperéue

### `comment`

Description de la carte

## Champs pour les noeuds

### `physic`

- ghost
  L'element na pas de crop physique, il est traversable

- box
  L'element a un crop physique en boit, l'attribut size défine le rayon de la
  taille de la boite, masse défine sa masse

- sphere
  L'element a un crop physique en sphehe, l'attribut size défine le rayon de la
  taille de la sphere, masse défine sa masse

- convex
  L'element a un crop physique convex calculer a parti de tout les pixele du
  maillaige, l'attribut masse défine sa masse

- tree
  L'element a un corp physique qui correspond exactement a sa formation,
  les element de ce type sont toujour statique (masse = 0)

### `size`

...

### `masse`

...
