# Exodus Population Simulation

The Bible reports that “The sons of Israel… kept on multiplying and growing at a very extraordinary rate.” (Ex 1:7) Over the 215 years the Israelites were in Egypt, they grew from around 50 men of military age, to about 600,000. (Ex 12:37; Nu 1:2, 3, 45, 46) This small simulation explores the following question—is this figure realistic?

The following parameters have been chosen, which can be edited in `ex_sim.h`:

| Parameter                   | Macro                    | Value   | Notes                                     |
|----------------------------|--------------------------|---------|--------------------------------------------|
| Starting male count        | `POP_START_COUNT`        | 50      | Initial male population                    |
| Starting male age mean     | `POP_START_MEAN_AGE`     | 15      | Mean starting ages                       |
| Starting male age stddev   | `POP_START_STDDEV_AGE`   | 4       | Standard deviation of start ages          |
| Minimum married age        | `MIN_MARRIED_AGE`        | 20      | Youngest a male can marry                  |
| Married age mean           | `MARRIED_AGE_MEAN`       | 22      | Average first marriage age                 |
| Married age stddev         | `MARRIED_AGE_STDDEV`     | 3       | Standard deviation for married age         |
| Max number of wives        | `MAX_WIVES`              | 3       | Polygamy limit                             |
| Annual extra wife odds     | `ANN_ADD_WIFE_ODDS`      | 0.012   | ~10% get an additional wife                |
| Male fertility end age     | `MALE_FERTILITY_END_AGE` | 40      | No children after this age                 |
| Annual child birth odds    | `ANN_CHILD_BIRTH_ODDS`   | 0.65    | Chance of child per year (per wife)        |
| Birth mortality odds       | `BIRTH_DEATH_ODDS`       | 0.1     | Risk of death at birth                     |
| Annual child death odds    | `ANN_CHILD_DEATH_ODDS`   | 0.006   | ~10% total child mortality                 |
| Annual adult death odds    | `ANN_ADULT_DEATH_ODDS`   | 0.001   | ~3% total adult mortality                  |

The fertility rate is slightly high, though still plausible. Mortality rates stand out as significantly low compared to historical norms. These are justified, given that the nation experienced divine aid. (Ge 46:3; Ps 105:24, 25) However, they can be adjusted to test alternative scenarios.

The simulation outputs the total number of military-age men, and the average number of sons per man.

Testing made possible by [munit](https://github.com/nemequ/munit).
