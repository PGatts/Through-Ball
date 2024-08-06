import pandas as pd
import LeagueScraper


country_codes_and_league_codes = {#used to create urls for every year of every league
    'eng': [['engprem', 2018, 2024], ['faprem', 1993, 2018], ['flcham', 2004, 2024], ['div1', 1999, 2004], ['flone', 2004, 2024], ['div2', 1999, 2004], ['fltwo', 2004, 2024], ['div3', 1999, 2004], ['national', 2015, 2024], ['confprem', 2007, 2015], ['confnat', 2004, 2007], ['confer', 2002, 2004]],
    'spain': [['spalali', 1995, 2024], ['spalali2', 2016, 2024], ['spasega', 2002, 2016]],
    'italy': [['serieb', 2002, 2024], ['seriea', 1995, 2024]],
    'ger': [['gerbun', 1995, 2024], ['gerbun2', 2001, 2024]],
    'france': [['fralig1', 2002, 2024], ['fradiv1', 1998, 2002], ['fralig2', 2002, 2024]],
    'netherl': [['nethere', 2011, 2024]],
    'holland': [['holere', 2001, 2011]],
    'scots': [['scotsp', 1998, 2024], ['scotsch', 2014, 2024]],
    'portugal': [['porprim', 2012, 2024], ['porsuper', 2001, 2012]],
    'belgium': [['belpro', 2009, 2016], ['beleers', 2004, 2009], ['beleers', 2016, 2023], ['belpro', 2023, 2024]],
    'turkey': [['tursuper', 2004, 2024]],
    'greece': [['gresuper', 2006, 2024], ['greethn', 2005, 2006]],
    'russia': [['russiapl', 2018, 2024], ['russiafpl', 2005, 2018]],
    'ukraine': [['ukrainepl', 2005, 2024]],
    'austria': [['ausbun', 2004, 2024]],
    'switz': [['switzsup', 2004, 2024]],
    'denmark': [['densuper', 2004, 2024]],
    'poland': [['polekstr', 2005, 2024]],
    'czech': [['cze1liga', 2012, 2024], ['czegamb', 2005, 2012]],
    'croatia': [['cro1hnl', 2022, 2024]],
    'hungary': [['hungnb1', 2012, 2024], ['hungotp', 2011, 2012], ['hungmoni', 2010, 2011], ['hungsopr', 2007, 2010], ['hungbors', 2005, 2007]],
    'norway': [['norelite', 2017, 2024], ['tippe', 2003, 2017]],
    'sweden': [['swedalls', 2005, 2024]],
    'ireland': [['ireprm', 2004, 2024]],
    'brazil': [['bracamp', 2005, 2024]],
    'chile': [['chileprm', 2017, 2024], ['chileaper', 2015, 2017], ['chileclaus', 2015, 2017]],
    'colombia': [['colaper', 2017, 2020], ['colfinal', 2017, 2019]],
    'uruguay': [['uruprim', 2023, 2024], ['uruaper', 2017, 2020], ['uruclaus', 2017, 2019]],
    'usa': [['usamls', 2017, 2024], ['usamsl', 2001, 2017]],
    'mexico': [['mexaper', 2005, 2024], ['mexclaus', 2004, 2024], ['mexbicen', 2009, 2010]],
    'australia': [['ausalge', 2005, 2024], ['ausprm', 2001, 2004]],
    'japan': [['japjlge', 2005, 2024]],
    'china': [['chinacsl', 2006, 2022]],
    'arg': [['argprim', 2015, 2017], ['argsuper', 2017, 2020], ['argprim', 2021, 2024], ['arginic', 2012, 2014], ['argfinal', 2012, 2014], ['argaper', 2005, 2012], ['argclaus', 2004, 2012], ['argtrans', 2014, 2015]]
}


all_leagues = []
#build all_leagues to contain all the lists of leagues, teams, and players using helper functions
for country, leagues_list in country_codes_and_league_codes.items():
    for league in leagues_list:
        all_leagues += LeagueScraper.get_league(country, league[0], league[1], league[2])

#create parallel arrays for each teammate pairing
froms = []
tos = []
for league_year in all_leagues:
    for team in league_year:
        for i in range(len(team)):
            for j in range(i + 1, len(team)):
                froms.append(team[i])
                tos.append(team[j])

#write the pairs to csv file format
df = pd.DataFrame(data={'From': froms, 'To': tos})
df.to_csv('through_ball_edge_list.csv', index=False)