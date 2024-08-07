This project aims to display connections between soccer players in the database found at [this link](https://www.footballsquads.co.uk/archive.htm) using two different graph algorithms which can be compared to one another.
The link in 'link.txt' must be used to download the csv file where data is stored. This should be saved to the same directory as main.cpp in order to run smoothly.
To run the code you're going to want to install a few things on your device. You first want to make sure you have homebrew and git installed Then use the following in your terminal: 
brew install boost openssl
brew install crowdin
brew install asio
brew install nlohmann-json
Then insert the csv found in 'link.txt' in the project.
If you get a message saying "Error: Could not open the file " when you run the code.
Try going to the main.cpp file in the main function and where it says "../through_ball_edge_list.csv", delete the "../"




