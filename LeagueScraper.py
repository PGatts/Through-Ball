import requests
import re
from bs4 import BeautifulSoup

def get_team(url):
    """
    :param url: url for the team to search
    :return: array of strings in which each string takes the form 'name dd-mm-yyyy'
    """
    #get html content from website
    response = requests.get(url)
    soup = BeautifulSoup(response.text, 'html.parser')

    table_rows = soup.find_all('tr')
    labels = table_rows[0]
    table_rows.remove(labels) #remove the row describing the contents of the table
    players = []

    #regex for dates in dd-mm-yy format
    dob_regex = re.compile(r'\d\d-\d\d-\d\d')
    try:
        #find the first row with complete data/not empty
        first_player_index = 0
        while (table_rows[first_player_index].find_all('td')[1].text + ' ').isspace() or (table_rows[first_player_index].find_all('td')[5].text + ' ').isspace():
            first_player_index += 1

        #find which column birthdays are in
        dob_index = 0
        first_row = table_rows[first_player_index].find_all('td')
        while str(first_row[dob_index].text).find('-') == str(first_row[dob_index].text).rfind('-'): #check for two '-' characters in string
            dob_index += 1

        #iterates through the table row by row
        for row in table_rows:
                tds = row.find_all('td')
                name = tds[1]
                dob = tds[dob_index]

                if name.text.strip() != "": #add player + dob to list
                    name_string = name.text.strip()
                    #remove any special characters that might be present in html
                    name_string = name_string.replace('\n', ' ').replace('\xa0', ' ').replace('\r', ' ').replace('\t', ' ').replace('\u2063', '').replace('     ',' ').replace('    ', ' ').replace('   ', ' ').replace('  ', ' ')
                    dob_string = dob.text.strip()
                    if re.search(dob_regex, dob_string) == None: #add default birthday of '00-00-00' if it's missing
                        dob_string += '00-00-00'
                    players.append(name_string + " " + dob_string) #add player to list
    except Exception as e:
        #returns when the end of the table is reached
        return players

    return players


def get_league(country_code, league_code, first_year, last_year):
    """
    :param country_code: How the country is identified on footballsquads.co.uk
    :param league_code: How the league is identified on footballsquads.co.uk
    :param first_year: Beginning of the range of years to search
    :param last_year: End of the range of years to search
    :return: A list of lists of strings where each outer list contains every team from a specific year of the league and each inner list contains the players from each team
    """
    league_players_by_year = []
    for i in range(first_year, last_year):
        #determines url structure for later on
        one_year = False
        if country_code in ['norway', 'sweden', 'ireland', 'brazil', 'colombia', 'uruguay', 'usa', 'japan', 'china'] or (league_code == 'argprim' and i in [2014, 2015, 2016, 2021, 2022, 2023]) or (league_code == 'russiafpl' and i in range(2005, 2011)) or (league_code == 'chileprm') or (league_code == 'argtrans'):
            one_year = True
            url = "https://www.footballsquads.co.uk/" + country_code + "/" + str(i) + "/" + league_code + ".htm"
        else:
            url = "https://www.footballsquads.co.uk/" + country_code + "/" + str(i) + "-" + str(i+1) + "/" + league_code + ".htm"
        response = requests.get(url)
        print(url)
        if response.status_code == 200:
            html_content = response.text
            soup = BeautifulSoup(html_content, 'html.parser')
            links = soup.find_all('a') #get all links on the page
            links.reverse() #reverse because the teams' links are the last on the page
            suffixes = []

            for j in range(len(soup.find_all('h5'))): #all h5 tags contain team names, thus their links point to team pages
                suffixes.append(links[j]['href'])

            players=[]
            for suffix in suffixes: #get players from each team in the season
                if one_year:
                    url = "https://www.footballsquads.co.uk/" + country_code + "/" + str(i) + "/" + suffix
                else:
                    url = "https://www.footballsquads.co.uk/" + country_code + "/" + str(i) + "-" + str(i+1) + "/" + suffix
                team_players = get_team(url)
                if team_players == []:
                    players.append(url)
                players.append(team_players) #add each team to the list containing all teams from that year
            league_players_by_year.append(players)

        else:
            print("Could not find " + url)
    return league_players_by_year