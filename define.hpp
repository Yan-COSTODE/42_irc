#pragma once
#ifndef DEFINE_HPP
#define DEFINE_HPP

#define RPL(code) ":localhost " + string(code) + " "
#define RPL_WELCOME(client) RPL("001") + client + " :Welcome to the IRC Network, " + client
#define RPL_NOTOPIC(client, channel) RPL("331") + client + " " + channel + " :No topic is set"
#define RPL_TOPIC(client, channel, topic) RPL("332") + client + " " + channel + " :" + topic
#define RPL_TOPICWHOTIME(client, channel, nick, setat) RPL("333") + client + " " + channel + " " + nick + " " + setat
#define RPL_INVITING(client, nick, channel) RPL("341") + client + " " + nick + " " + channel
#define RPL_INVITED(client, nick, channel) ":" + client + " INVITE " + nick + " :" + channel;
#define RPL_QUIT(client, reason) ":" + client + " QUIT :Quit: " + reason;
#define RPL_JOIN(client, channel) ":" + client + " JOIN " + channel;
#define RPL_NAMEREPLY(client, symbol, channel, prefix, nick) RPL("353") + client + " " + symbol + " " + channel + " :" + prefix + nick
#define RPL_ENDOFNAMES(client, channel) RPL("366") + client + " " + channel + " :End of /NAMES list"
#define RPL_LIST(client, channel, client_count, topic) RPL("322") + client + " " + channel + " " + client_count + " :" + topic
#define RPL_LISTEND(client) RPL("323") + client + " :End of /LIST"
#define RPL_KICK(client, channel, nick, reason) ":" + client + " KICK " + channel + " " + nick + " :" + reason;
#define RPL_PRIVMSG(client, nick, text) ":" + client + " PRIVMSG " + nick + " :" + text;
#define RPL_PART(client, channel, reason) ":" + client + " PART " + channel + " :" + reason;
#define RPL_NICK(client, nick) ":" + client + " NICK " + nick;
#define RPL_PONG(token) ":localhost PONG localhost " + token;
#define RPL_HELPSTART(client, subject, line) RPL("704") + client + " " + subject + " :" + line + "\n"
#define RPL_HELPTXT(client, subject, line) RPL("705") + client + " " + subject + " :" + line + "\n"
#define RPL_ENDOFHELP(client, subject, line) RPL("706") + client + " " + subject + " :" + line
#define RPL_MODE(client, channel, mode) ":" + client + " MODE " + channel + " " + mode;

#define ERR_NEEDMOREPARAMS(client, command) RPL("461") + client + " " + command + " :Not enough parameters"
#define ERR_NOSUCHCHANNEL(client, channel) RPL("403") + client + " " + channel + " :Not such channel"
#define ERR_NOTONCHANNEL(client, channel) RPL("442") + client + " " + channel + " :You're not on that channel"
#define ERR_USERONCHANNEL(client, nick, channel) RPL("443") + client + " " + nick + " " + channel + " :is already on channel"
#define ERR_USERNOTINCHANNEL(client, nick, channel) RPL("441") + client + " " + nick + " " + channel + " :They aren't on that channel"
#define ERR_CHANOPRIVSNEEDED(client, channel) RPL("482") + client + " " + channel + " :You're not channel operator"
#define ERR_NONICKNAMEGIVEN(client) RPL("431") + client + " :No nickname given"
#define ERR_ERROREUSNICKNAME(client, nick) RPL("432") + client  + " " + nick + " :Errorneus nickname"
#define ERR_NICKNAMEINUSE(client, nick) RPL("433") + client  + " " + nick + " :Nickname is already in use"
#define ERR_ALREADYREGISTERED(client) RPL("462") + client + " :You may not reregister"
#define ERR_PASSWDMISMATCH(client) RPL("464") + client + " :Password incorrect"
#define ERR_INVITEONLYCHAN(client, channel) RPL("473") + client + " " + channel + " :Cannot join channel (+i)"
#define ERR_CHANNELISFULL(client, channel) RPL("471") + client + " " + channel + " :Cannot join channel (+l)"
#define ERR_BADCHANNELKEY(client, channel) RPL("475") + client + " " + channel + " :Cannot join channel (+k)"
#define ERR_NOSUCHNICK(client, nick) RPL("475") + client + " " + nick + " :No such nick/channel"
#define ERR_NORECIPIENT(client, command) RPL("411") + client + " :No recipient given (" + command + ")"
#define ERR_CANNOTSENDTOCHAN(client, channel) RPL("404") + client + " " + channel + " :Cannot send to channel"
#define ERR_NOTEXTTOSEND(client) RPL("412") + client + " :No text to send"
#define ERR_UNKNOWNERROR(client, command, info) RPL("400") + client + " " + command + " :" + info
#define ERR_HELPNOTFOUND(client, subject) RPL("524") + client + " " + subject + " :No help available on this topic"
#define ERR_UMODEUNKNOWNFLAG(client) RPL("501") + client + " :Unknown MODE flag"

#endif