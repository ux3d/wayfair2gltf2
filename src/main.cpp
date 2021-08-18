#include <cstdio>
#include <fstream>
#include <string>

#include <nlohmann/json.hpp>

#include <curl/curl.h>

using json = nlohmann::json;

bool saveFile(const std::string& output, const std::string& filename)
{
	std::ofstream file(filename, std::ios::binary);
	if (!file.is_open())
	{
		return false;
	}

	file.write(output.data(), output.size());
	file.close();

	return true;
}

size_t CurlWrite(void* contents, size_t size, size_t nmemb, std::string* s)
{
    size_t newLength = size * nmemb;

    try
    {
        s->append((char*)contents, newLength);
    }
    catch(std::bad_alloc& e)
    {
        return 0;
    }

    return newLength;
}

int main(int argc, char *argv[])
{
	std::string demoURI = "https://api.wayfair.com/v1/3dapi/models_demo";
	std::string registeredURI = "https://api.wayfair.com/v1/3dapi/models";
	std::string URI = demoURI;

	std::string demoSKU = "ZPCD5744";
	std::string registeredSKU = "FV50959";
	std::string SKU = demoSKU;

	std::string email = "";
	std::string password = "";
	std::string userSKU = "";

	bool verbose = false;

	//

	for (int i = 0; i < argc; i++)
	{
		if (strcmp(argv[i], "-e") == 0 && (i + 1 < argc))
		{
			email = argv[i + 1];
		}
		if (strcmp(argv[i], "-p") == 0 && (i + 1 < argc))
		{
			password = argv[i + 1];
		}
		else if (strcmp(argv[i], "-s") == 0 && (i + 1 < argc))
		{
			userSKU = argv[i + 1];
		}
		else if (strcmp(argv[i], "-v") == 0 && (i + 1 < argc))
		{
			if (strcmp(argv[i + 1], "true") == 0)
			{
				verbose = true;
			}
			else if (strcmp(argv[i + 1], "false") == 0)
			{
				verbose = false;
			}
		}
	}

	//

	if (email != "")
	{
		URI = registeredURI;
		SKU = registeredSKU;
	}

	if (userSKU != "")
	{
		SKU = userSKU;
	}

	//

    CURL* curl = nullptr;
    CURLcode result;
    std::string response = "";

    curl_global_init(CURL_GLOBAL_DEFAULT);

    curl = curl_easy_init();

    if (!curl)
    {
		printf("Error: Could not initialize curl'\n");

		return -1;
    }

    printf("Info: Initialized curl'\n");

    //

	curl_easy_setopt(curl, CURLOPT_URL, URI.c_str());

	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CurlWrite);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
	if (verbose)
	{
		curl_easy_setopt (curl, CURLOPT_VERBOSE, 1L);
	}
	if (email != "")
	{
		curl_easy_setopt(curl, CURLOPT_USERNAME, email.c_str());
		curl_easy_setopt(curl, CURLOPT_PASSWORD, password.c_str());
		curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
	}

	printf("Info: Calling URL '%s'\n", URI.c_str());

	result = curl_easy_perform(curl);

	if(result != CURLE_OK)
	{
		printf("Error: curl_easy_perform() failed: %s\n", curl_easy_strerror(result));

		curl_easy_cleanup(curl);

		return -1;
	}

    //

    json jsonData = json::parse(response);

    printf("Info: Searching SKU '%s'\n\n", SKU.c_str());

    for (auto element : jsonData)
    {
    	std::string currentSKU = "";
    	bool foundSKU = false;

    	if (element.contains("sku"))
    	{
    		json& sku = element["sku"];

    		currentSKU = sku.get<std::string>();

    		printf("Info: Processing SKU '%s'\n", currentSKU.c_str());

        	// Avoid mass download
    		if (currentSKU == SKU)
    		{
    			printf("Info: Found SKU '%s'\n", currentSKU.c_str());

    			foundSKU = true;
    		}
    		else
    		{
    			foundSKU = false;
    		}
    	}
    	else
    	{
    		foundSKU = false;

    		continue;
    	}

    	//

    	if (element.contains("model"))
    	{
    		json& model = element["model"];

    		if (model.contains("glb"))
    		{
    			json& glb = model["glb"];

    			if (!glb.is_null())
    			{
    		    	printf("Info: GLB uri available\n");

    				if (foundSKU)
    				{
						std::string glb_uri = glb.get<std::string>();
						std::string glb_data = "";

						printf("Info: Found glTF '%s'\n", glb_uri.c_str());

						curl_easy_setopt(curl, CURLOPT_URL, glb_uri.c_str());

						curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
						curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
						curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CurlWrite);
						curl_easy_setopt(curl, CURLOPT_WRITEDATA, &glb_data);

						printf("Info: Downloading GLB '%s'\n", glb_uri.c_str());

						result = curl_easy_perform(curl);

						if(result != CURLE_OK)
						{
							printf("Error: Could not download GLB '%s'\n", curl_easy_strerror(result));

							continue;
						}

						//

						std::string filename = glb_uri;
						size_t index = filename.find_last_of('/');
						if (index != std::string::npos)
						{
							filename = filename.substr(index + 1);
						}

						if (!saveFile(glb_data, filename))
						{
							printf("Error: Could not save GLB '%s'\n", filename.c_str());

							continue;
						}

						printf("Info: Saved GLB '%s'\n", filename.c_str());
    				}

    				printf("\n");
    			}
    			else
    			{
    				printf("Info: No GLB uri available\n\n");
    			}
    		}
			else
			{
				printf("Error: No GLB available\n\n");
			}
    	}
		else
		{
			printf("Error: No model available\n\n");
		}
    }

    curl_easy_cleanup(curl);

    printf("Info: Done\n");

    return 0;
}
