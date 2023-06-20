
#include "Syntax.hpp"
#include "Exception.hpp"
#include "Utils.hpp"

#include <sstream> // stringstream
#include <algorithm> // replace if
#include <stack>
#include <sys/time.h>

/**
 * @brief trim line from # until the end
 * 
 * @param str 
 * @return std::string 
 */
std::string		Syntax::trimComments(const std::string &str) {
	std::string new_line;
	size_t hash_char_pos;

	new_line = str;
	hash_char_pos = new_line.find('#');
	if (hash_char_pos == std::string::npos)
		return new_line;
	return new_line.substr(0, hash_char_pos);
}

/**
 * @brief Delete extra whitespaces if more than one
 * 
 * @param str 
 * @return std::string 
 */
std::string		Syntax::replaceConsecutiveSpaces(const std::string& str) 
{
    std::stringstream ss;
    bool previousCharIsSpace = false;
    for (std::string::const_iterator it = str.begin(); it!=str.end(); ++it) 
	{
        char c = *it;
        if ( !isspace(c) || !previousCharIsSpace)
            ss << c;
        previousCharIsSpace = isspace(c);
    }
    return ss.str();
}

/**
 * @brief Trim whitespaces at beginning, at end of line, 
 *	and replaces chain of whitespaces by a single space
 *	return : std::string
 * 
 * @param str 
 * @return std::string 
 */
std::string		Syntax::trimWhitespaces(const std::string& str)
{
	std::string whitespaces;
	std::string new_line;
	size_t start, end;

	whitespaces = WHITESPACES;
	new_line = str;
	start = new_line.find_first_not_of(whitespaces);
	if (start != std::string::npos)
		new_line = new_line.substr(start);
	end = new_line.find_last_not_of(whitespaces);
	if (end != std::string::npos)
		new_line = new_line.substr(0, end + 1);
	new_line = replaceConsecutiveSpaces(new_line);
	std::replace_if(new_line.begin(), new_line.end(), isspace, ' ');
	return new_line;
}

/**
 * @brief gets a line into a line filles with "\n"
 * given a line number
 * @param str 
 * @param n quelle ligne?
 * @return std::string 
 */
std::string		Syntax::getLine(std::string str, size_t n)
{
	int i = 0;
	size_t j = 0;
	size_t ct = 0;
	std::string temp;

	if (n >= nbLines(str))
		return std::string();
	while (ct < n)
	{
		if (str[i++] =='\n')
			ct++;
	}
	while (std::isspace(str[i]) && str[i] != '\n')
		++i;
	while (str[i + j] && str[i + j] != '\n')
		j++;
	while (j > 0 && std::isspace(str[i + j - 1]))
		--j;
	temp = std::string(str, i, j);
	return (temp);
}

/**
 * @brief return  number of lines in the string (\n separated)
 * 
 * @param str_config 
 * @return size_t 
 */
size_t			Syntax::nbLines(std::string &str_config)
{
	size_t lines = 1;
	for (std::string::iterator ite = str_config.begin(); ite!=str_config.end(); ite++)
		if (*ite == '\n')
			lines++;
	return (lines);
}

/**
 * @brief Check if the line from conf file is of interest 
 * ie size > 0 and not }
 * @param str 
 * @param pos 
 * @return true 
 * @return false 
 */
bool			Syntax::isNothing(std::string str, int pos)
{
	std::string line;

	line = Syntax::getLine(str, pos);
	return (line.size() == 0 || line[0] == '}');
}

/**
 * @brief Check if brackets opened/closed is correct
 * 
 * @param config_string 
 * @return true 
 * @return false 
 */
bool			Syntax::checkBrackets(std::string &config_string)
{
	std::stack<char>	bracket;
	std::string::iterator ite;
	ite = config_string.begin();
	for (;ite != config_string.end();ite++)
	{
		if (*ite == '{')
			bracket.push(*ite);
		if (*ite == '}')
		{
			if (bracket.empty() || bracket.top() != '{')
				return false;
			if (!bracket.empty() && bracket.top() == '{')
				bracket.pop();
		}
	}
	if (bracket.empty())
		return true;
	return false;
}

/**
 * @brief 
 * test if path to .conf is correct, testing
 * emptypath, extension, readable, if directory
 * trying to open file
 * @param path 
 */
void			Syntax::testPath(const std::string &path)
{
	size_t ext_pos;
	std::ifstream file;
	
	if (path.empty())
		throw (EmptyConfPath());
	ext_pos = path.find(".conf");
	if (ext_pos == std::string::npos || ext_pos != path.size() - 5)
		throw (BadExtensionConfFile());
	if (!Utils::fileExists(path.c_str()))
		throw (FileDoesNotExist());
	if (!Utils::fileRead(path.c_str()))
		throw (FileNotReadable());
	if (Utils::isDirectory(path.c_str()))
		throw (PathIsDir());
	file.open(path.c_str(), std::ios_base::in);
	if (!file) {
		throw (InvalidConfFilePath());
	}
}

/**
 * @brief gets a line and returns a line, trimming the first i lines
 * 
 * @param string 
 * @param position
 * @return std::string 
 */
std::string 	Syntax::trimLineToI(std::string &str, size_t pos)
{
	int i = 0;
	size_t ct = 0;
	std::string temp;

	if (pos >= nbLines(str))
		return temp;
	while (ct < pos)
	{
		if (str[i++] =='\n')
			ct++;
	}
	temp = std::string(str, i);
	return (temp);
}

/**
 * @brief Finds the closing bracket line in the 
 * string where the first opening bracket has already been
 * removed: needs to checked pairs of included brackets
 * 
 * @param str 
 * @return int 
 */
int				Syntax::findClosingBracket(std::string str)
{
	int line = 1;
	int count = 1;
	for (size_t i = 0; i < nbLines(str); i++)
	{
		if (getLine(str, i).find("{")!= std::string::npos)
			count +=1;
		if (getLine(str, i).find("}")!= std::string::npos)
		{
			if ((count -=1) == 0)
				return line;
		}
		line ++;
	}
	return (-1);
}

/**
 * @brief splits a string around a character in the given charset
 * 
 * @param str 
 * @param charset 
 * @return std::vector<std::string> 
 */
std::vector<std::string> Syntax::splitString(std::string str, const std::string &charset)
{
	std::vector<std::string> tokens;
    size_t pos = 0;
    std::string token;

    while ((pos = str.find_first_of(charset)) != std::string::npos)
	{
        token = str.substr(0, pos);
        if(token.compare(""))
			tokens.push_back(token);
        str.erase(0, pos + 1);
    }
    tokens.push_back(str);
    return tokens;
}

std::string Syntax::intToString(int num) {
  std::stringstream ss;
  ss << num;
  return ss.str();
}

/**
 * @brief makes sure the conf file lines finishes by ;
 * and { are isolated in one line
 * deleting the single \n
 * 
 * @param std::string &conf 
 */
void Syntax::formatConfFile(std::string &conf)
{
	int i = 0;
	int j = conf.size();
	while (i < j){
		if (conf[i] == ';'){
			conf.replace(i, 1, ";\n");
			j++;
		}
		i++;
	}
	i = 0;
	j = conf.size();
	while (i < j){
		if (conf[i] == '{'){
			conf.replace(i, 1, "\n{\n");
			j +=2;
			i +=2;
		}
		else i++;
	}
	i = 0;
	j = conf.size();
	while (i < j){
		if (conf[i] == '}'){
			conf.replace(i, 1, "\n}\n");
			j+=2;
			i+=2;
		}
		else
			i++;
	}
	i = 0;
	j = conf.size();
	while (i < j){
		if (conf[i] == '\n' && conf[i+1] && conf[i+1] == '\n'){
			conf.replace(i, 2, "\n");
			j-=1;
			i-=1;
		}
		else
			i++;
	}
}

/**
 * @brief tool used to format properly the conf string
 * returning \n or " "
 * 
 * @param str 
 * @return char 
 */
char Syntax::checkChar(std::string str)
{
		if (str[str.size()-1] == ';')
			return '\n';
		if (!str.compare("server") || !str.compare("{") || !str.compare("}") )
			return '\n';
		return (' ');
}

/**
	 * @brief Checks if the server directive are part of server or location block
	 * 
	 * @param token 
	 * @return true 
	 * @return false 
	 */
	int 	Syntax::correctServerInstruction(std::vector<std::string> token)
	{
				int i = 0;
		// check if the token corresponds to a valid instruction in server block
		while (i < TOTAL_SERVER_INSTRUCTIONS)
		{
			if (!token[0].compare(Syntax::server_instructions_tab[i].name))
				return i;
			i++;
		}
		if (Syntax::isNothing(token[0]) || !token[0].compare("{"))
			return TOTAL_SERVER_INSTRUCTIONS;
		return -1;
}

/**
 * @brief Checks if the location directive are part of location block
 * 
 * @param token 
 * @return true 
 * @return false 
 */
int 	Syntax::correctLocationInstruction(std::vector<std::string> token)
{
	int i = 0;
	// check if the token corresponds to a valid instruction in server block
	while (i < TOTAL_LOCATION_INSTRUCTIONS)
	{
		if (!token[0].compare(Syntax::location_instructions_tab[i].name))
			return i;
		i++;
	}
	if (Syntax::isNothing(token[0]) || !token[0].compare("{"))
		return TOTAL_LOCATION_INSTRUCTIONS;
	return -1;}

int 	Syntax::correctMethodInstruction(std::vector<std::string> token)
{
	size_t i = 0;
	size_t j = 1;
	token[token.size() - 1] = token[token.size() - 1].erase(token[token.size() - 1].size() - 1);
	while (j < token.size())
	{
		i = 0;
		while (i < TOTAL_METHODS_INSTRUCTIONS)
		{
			if (!token[j].compare(Syntax::method_tab[i].name))
				return j;
			i++;
		}
		j ++;
	}
	return -1;
}

/**
 * @brief lists all methods instructions
 * for use in the table of functions
 * 
 */
const Syntax::method_tab_entry_t
Syntax::method_tab[] = {
	{GET, "GET"},
	{HEAD, "HEAD"},
	{POST, "POST"},
	{PUT, "PUT"},
	{DELETE, "DELETE"},
	{CONNECT, "CONNECT"},
	{OPTIONS, "OPTIONS"},
	{TRACE, "TRACE"},
};

/**
 * @brief lists all server instructions
 * for use in the table of functions
 * 
 */
const Syntax::server_instruction_tab_entry_t
Syntax::server_instructions_tab[] = {
	{S_ROOT, "root"},
	{LISTEN, "listen"},
	{SERVER_NAME, "server_name"},
	{S_ERROR_PAGE, "error_page"},
	{S_INDEX, "index"},
	{S_AUTOINDEX, "autoindex"},
	{S_CLIENT_MAX_BODY_SIZE, "client_max_body_size"},
	{S_CGI, "cgi"},
	{LOCATION_INSTRUCTION, "location"}
};


/**
 * @brief lists all location instructions
 * for use in the table of functions
 * 
 */
const Syntax::location_instruction_tab_entry_t
Syntax::location_instructions_tab[] = {
	{L_ROOT, "root"},
	{METHODS, "methods"},
	{L_INDEX, "index"},
	{L_CGI, "cgi"},
	{L_AUTOINDEX, "autoindex"},
	{UPLOAD_DIR, "upload_dir"},
	{RETURN,"return"},
	{L_CLIENT_MAX_BODY_SIZE, "client_max_body_size"},
	{L_ERROR_PAGE, "error_page"},
	{URI, "uri"},
};

const Syntax::request_header_tab_entry_t
Syntax::request_header_tab[] = {
	{ACCEPT_CHARSET, "Accept-Charset"},
	{CONTENT_LENGTH, "Content-Length"},
	{AUTHORIZATION, "Authorization"},
	{CONTENT_TYPE, "Content-Type"},
	{DATE, "Date"},
	{HOST, "Host"},
	{REFERER, "Referer"},
	{TRANSFER_ENCODING, "Transfer-Encoding"},
	{USER_AGENT, "User-Agent"},
};

const Syntax::answer_header_tab_entry_t
Syntax::answer_header_tab[] = {
	{ALLOW, "Allow"},
	{CONTENT_LANGUAGE, "Content-Language"},
	{A_CONTENT_LENGTH, "Content-Length"},
	{CONTENT_LOCATION, "Content-Location"},
	{A_CONTENT_TYPE, "Content-Type"},
	{A_DATE, "Date"},
	{LAST_MODIFIED, "Last-Modified"},
	{LOCATION, "Location"},
	{RETRY_AFTER, "Retry-After"},
	{SERVER, "Server"},
	{A_TRANSFER_ENCODING, "Transfer-Encoding"},
	{WWW_AUTHENTICATE, "WWW-Authenticate"},
};

void Syntax::fill_response_status_map(std::map<status_code_t, std::string> &map)
{
	map.insert(std::pair<status_code_t, std::string>(CONTINUE,"Continue"));
	map.insert(std::pair<status_code_t, std::string>(SWITCHING_PROTOCOLS,"Switching Protocols"));
	map.insert(std::pair<status_code_t, std::string>(OK,"OK"));
	map.insert(std::pair<status_code_t, std::string>(CREATED,"Created"));
	map.insert(std::pair<status_code_t, std::string>(ACCEPTED,"Accepted"));
	map.insert(std::pair<status_code_t, std::string>(NON_AUTHORITATIVE_INFORMATION,"Non-Authoritative Information"));
	map.insert(std::pair<status_code_t, std::string>(NO_CONTENT,"No Content"));
	map.insert(std::pair<status_code_t, std::string>(RESET_CONTENT,"Reset Content"));
	map.insert(std::pair<status_code_t, std::string>(PARTIAL_CONTENT,"Partial Content"));
	map.insert(std::pair<status_code_t, std::string>(MULTIPLE_CHOICES,"Multiple Choices"));
	map.insert(std::pair<status_code_t, std::string>(MOVED_PERMANENTLY,"Moved Permanently"));
	map.insert(std::pair<status_code_t, std::string>(FOUND,"Found"));
	map.insert(std::pair<status_code_t, std::string>(SEE_OTHER,"See Other"));
	map.insert(std::pair<status_code_t, std::string>(NOT_MODIFIED,"Not Modified"));
	map.insert(std::pair<status_code_t, std::string>(USE_PROXY,"Use Proxy"));
	map.insert(std::pair<status_code_t, std::string>(TEMPORARY_REDIRECT,"Temporary Redirect"));
	map.insert(std::pair<status_code_t, std::string>(BAD_REQUEST,"Bad Request"));
	map.insert(std::pair<status_code_t, std::string>(UNAUTHORIZED,"Unauthorized"));
	map.insert(std::pair<status_code_t, std::string>(PAYMENT_REQUIRED,"Payment Required"));
	map.insert(std::pair<status_code_t, std::string>(FORBIDDEN,"Forbidden"));
	map.insert(std::pair<status_code_t, std::string>(NOT_FOUND,"Not Found"));
	map.insert(std::pair<status_code_t, std::string>(METHOD_NOT_ALLOWED,"Method Not Allowed"));
	map.insert(std::pair<status_code_t, std::string>(NOT_ACCEPTABLE,"Not Acceptable"));
	map.insert(std::pair<status_code_t, std::string>(PROXY_AUTHENTICATION_REQUIRED,"Proxy Authentication Required"));
	map.insert(std::pair<status_code_t, std::string>(REQUEST_TIMEOUT,"Request Timeout"));
	map.insert(std::pair<status_code_t, std::string>(CONFLICT,"Conflict"));
	map.insert(std::pair<status_code_t, std::string>(GONE,"Gone"));
	map.insert(std::pair<status_code_t, std::string>(LENGTH_REQUIRED,"Length Required"));
	map.insert(std::pair<status_code_t, std::string>(PRECONDITION_FAILED,"Precondition Failed"));
	map.insert(std::pair<status_code_t, std::string>(PAYLOAD_TOO_LARGE,"Payload Too Large"));
	map.insert(std::pair<status_code_t, std::string>(URI_TOO_LONG,"URI Too Long"));
	map.insert(std::pair<status_code_t, std::string>(UNSUPPORTED_MEDIA_TYPE,"Unsupported Media Type"));
	map.insert(std::pair<status_code_t, std::string>(RANGE_NOT_SATISFIABLE,"Range Not Satisfiable"));
	map.insert(std::pair<status_code_t, std::string>(EXPECTATION_FAILED,"Expectation Failed"));
	map.insert(std::pair<status_code_t, std::string>(INTERNAL_SERVER_ERROR,"Internal Server Error"));
	map.insert(std::pair<status_code_t, std::string>(NOT_IMPLEMENTED,"Not Implemented"));
	map.insert(std::pair<status_code_t, std::string>(BAD_GATEWAY,"Bad Gateway"));
	map.insert(std::pair<status_code_t, std::string>(SERVICE_UNAVAILABLE,"Service Unavailable"));
	map.insert(std::pair<status_code_t, std::string>(GATEWAY_TIMEOUT,"Gateway Timeout"));
	map.insert(std::pair<status_code_t, std::string>(HTTP_VERSION_NOT_SUPPORTED,"HTTP Version Not Supported"));
}

void Syntax::fillMimeTypeMap(std::map<std::string, std::string> &map)
 {
	map.insert(std::pair<std::string, std::string>(".aac", "audio/aac"));
	map.insert(std::pair<std::string, std::string>(".abw", "application/x-abiword"));
	map.insert(std::pair<std::string, std::string>(".arc", "application/octet-stream"));
	map.insert(std::pair<std::string, std::string>(".avi", "video/x-msvideo"));
	map.insert(std::pair<std::string, std::string>(".azw", "application/vnd.amazon.ebook"));
	map.insert(std::pair<std::string, std::string>(".bin", "application/octet-stream"));
	map.insert(std::pair<std::string, std::string>(".bmp", "image/bmp"));
	map.insert(std::pair<std::string, std::string>(".bz", "application/x-bzip"));
	map.insert(std::pair<std::string, std::string>(".bz2", "application/x-bzip2"));
	map.insert(std::pair<std::string, std::string>(".csh", "application/x-csh"));
	map.insert(std::pair<std::string, std::string>(".css", "text/css"));
	map.insert(std::pair<std::string, std::string>(".csv", "text/csv"));
	map.insert(std::pair<std::string, std::string>(".txt", "text/plain"));
	map.insert(std::pair<std::string, std::string>(".doc", "application/msword"));
	map.insert(std::pair<std::string, std::string>(".docx", "application/vnd.openxmlformats-officedocument.wordprocessingml.document"));
	map.insert(std::pair<std::string, std::string>(".eot", "application/vnd.ms-fontobject"));
	map.insert(std::pair<std::string, std::string>(".epub", "application/epub+zip"));
	map.insert(std::pair<std::string, std::string>(".gif", "image/gif"));
	map.insert(std::pair<std::string, std::string>(".htm", "text/html"));
	map.insert(std::pair<std::string, std::string>(".html", "text/html"));
	map.insert(std::pair<std::string, std::string>(".ico", "image/x-icon"));
	map.insert(std::pair<std::string, std::string>(".ics", "text/calendar"));
	map.insert(std::pair<std::string, std::string>(".jar", "application/java-archive"));
	map.insert(std::pair<std::string, std::string>(".jpeg", "image/jpeg"));
	map.insert(std::pair<std::string, std::string>(".jpg", "image/jpeg"));
	map.insert(std::pair<std::string, std::string>(".js", "application/javascript"));
	map.insert(std::pair<std::string, std::string>(".json", "application/json"));
	map.insert(std::pair<std::string, std::string>(".mid", "audio/midi"));
	map.insert(std::pair<std::string, std::string>(".midi", "audio/midi"));
	map.insert(std::pair<std::string, std::string>(".mpeg", "video/mpeg"));
	map.insert(std::pair<std::string, std::string>(".mpkg", "application/vnd.apple.installer+xml"));
	map.insert(std::pair<std::string, std::string>(".odp", "application/vnd.oasis.opendocument.presentation"));
	map.insert(std::pair<std::string, std::string>(".ods", "application/vnd.oasis.opendocument.spreadsheet"));
	map.insert(std::pair<std::string, std::string>(".odt", "application/vnd.oasis.opendocument.text"));
	map.insert(std::pair<std::string, std::string>(".oga", "audio/ogg"));
	map.insert(std::pair<std::string, std::string>(".ogv", "video/ogg"));
	map.insert(std::pair<std::string, std::string>(".ogx", "application/ogg"));
	map.insert(std::pair<std::string, std::string>(".otf", "font/otf"));
	map.insert(std::pair<std::string, std::string>(".png", "image/png"));
	map.insert(std::pair<std::string, std::string>(".pdf", "application/pdf"));
	map.insert(std::pair<std::string, std::string>(".ppt", "application/vnd.ms-powerpoint"));
	map.insert(std::pair<std::string, std::string>(".pptx", "application/vnd.openxmlformats-officedocument.presentationml.presentation"));
	map.insert(std::pair<std::string, std::string>(".rar", "application/x-rar-compressed"));
	map.insert(std::pair<std::string, std::string>(".rtf", "application/rtf"));
	map.insert(std::pair<std::string, std::string>(".sh", "application/x-sh"));
	map.insert(std::pair<std::string, std::string>(".svg", "image/svg+xml"));
	map.insert(std::pair<std::string, std::string>(".swf", "application/x-shockwave-flash"));
	map.insert(std::pair<std::string, std::string>(".tar", "application/x-tar"));
	map.insert(std::pair<std::string, std::string>(".tif", "image/tiff"));
	map.insert(std::pair<std::string, std::string>(".tiff", "image/tiff"));
	map.insert(std::pair<std::string, std::string>(".ts", "application/typescript"));
	map.insert(std::pair<std::string, std::string>(".ttf", "font/ttf"));
	map.insert(std::pair<std::string, std::string>(".vsd", "application/vnd.visio"));
	map.insert(std::pair<std::string, std::string>(".wav", "audio/x-wav"));
	map.insert(std::pair<std::string, std::string>(".weba", "audio/webm"));
	map.insert(std::pair<std::string, std::string>(".webm", "video/webm"));
	map.insert(std::pair<std::string, std::string>(".webp", "image/webp"));
	map.insert(std::pair<std::string, std::string>(".woff", "font/woff"));
	map.insert(std::pair<std::string, std::string>(".woff2", "font/woff2"));
	map.insert(std::pair<std::string, std::string>(".xhtml", "application/xhtml+xml"));
	map.insert(std::pair<std::string, std::string>(".xls", "application/vnd.ms-excel"));
	map.insert(std::pair<std::string, std::string>(".xlsx", "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet"));
	map.insert(std::pair<std::string, std::string>(".xml", "application/xml"));
	map.insert(std::pair<std::string, std::string>(".xul", "application/vnd.mozilla.xul+xml"));
	map.insert(std::pair<std::string, std::string>(".zip", "application/zip"));
	map.insert(std::pair<std::string, std::string>(".3gp", "video/3gpp"));
	map.insert(std::pair<std::string, std::string>(".3g2", "video/3gpp2"));
	map.insert(std::pair<std::string, std::string>(".7z", "application/x-7z-compressed"));
}

std::string	Syntax::getFormattedDate(std::time_t time)
{
    char		date[100];
	int			ret;

    ret = std::strftime(date, sizeof(date), "%a, %d %b %Y %X GMT", std::localtime(&time));
	if (!ret)
	{
		throw (FatalError("Webserv error: strftime() function failed"));
	}
	return(std::string(date));	
}

long long	Syntax::getTimeOfDayMs(){
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return static_cast<long long>(tv.tv_sec) *1000LL + static_cast<long long> (tv.tv_usec)/1000LL;
}