
#ifndef _DWR_POLYFIT_H_
#define _DWR_POLYFIT_H_

typedef struct {
    double y_min;
    double y_max;
    double x0;
    double x1;
    double x2;
    double x3;
    double x4;
    double x5;
    double x6;
    double x7;
    double x8;
    double x9;
    double x10;
} polyfactors;

extern const polyfactors mon_hp_fac; /* monster HP */
extern const polyfactors mon_str_fac; /* monster strength */
extern const polyfactors mon_agi_fac; /* monster agility */
extern const polyfactors mon_xp_fac; /* xp drop */
extern const polyfactors mon_xp_fac; /* xp drop */
extern const polyfactors mon_gold_fac; /* gold drop */
extern const polyfactors mon_sr_fac; /* sleep resist */
extern const polyfactors mon_ssr_fac; /* stopspell resist */
extern const polyfactors mon_hr_fac; /* hurt resist */
extern const polyfactors mon_dodge_fac; /* dodge rate */
extern const polyfactors hero_hp_fac; /* hero hp */
extern const polyfactors hero_mp_fac; /* hero mp */
extern const polyfactors hero_str_fac; /* hero mp */
extern const polyfactors hero_agi_fac; /* hero agility */
extern const polyfactors hero_xp_fac; /* random level xp */
extern const polyfactors wpn_price_fac; /* weapon prices */

double polyfit(double x, const polyfactors *fac);

#endif
