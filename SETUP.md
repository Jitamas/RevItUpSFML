# RevItUpSFML - Setup Instructions

## Enhanced Car Parts Search

This application features a specialized search system designed specifically for finding automotive parts with pricing information.

### Search Features:
- **Precision Filtering**: Only shows results related to car/automotive parts
- **Price Extraction**: Automatically extracts and displays price information when available
- **Enhanced Queries**: Automatically adds automotive-related keywords to improve search accuracy
- **Smart Filtering**: Uses keyword matching to ensure results are relevant to automotive parts

### How the Search Works:
1. Your search term is enhanced with automotive keywords like "car parts", "automotive", "price", etc.
2. Results are filtered to only show automotive-related content
3. Price information is extracted from titles and descriptions using multiple price patterns
4. Results display: Title, Link, and Price (when available)

## Google Search API Setup

To use the Parts search functionality, you need to set up your own Google Search API credentials.

### Step 1: Get Google API Credentials

1. Go to [Google Cloud Console](https://console.cloud.google.com/)
2. Create a new project or select an existing one
3. Enable the "Custom Search JSON API"
4. Create an API Key:
   - Go to "Credentials" → "Create Credentials" → "API Key"
   - Copy your API key

### Step 2: Set Up Custom Search Engine

1. Go to [Google Custom Search Engine](https://cse.google.com/)
2. Create a new search engine
3. Set it to search the entire web or specific sites
4. Copy your Search Engine ID (CX)

### Step 3: Set Environment Variables

#### macOS/Linux:
```bash
export GOOGLE_API_KEY="your_api_key_here"
export GOOGLE_CX="your_search_engine_id_here"
```

To make them permanent, add to your shell profile:
```bash
echo 'export GOOGLE_API_KEY="your_api_key_here"' >> ~/.bash_profile
echo 'export GOOGLE_CX="your_search_engine_id_here"' >> ~/.bash_profile
```

#### Windows:
```cmd
set GOOGLE_API_KEY=your_api_key_here
set GOOGLE_CX=your_search_engine_id_here
```

### Step 4: Build and Run

1. Make sure SFML and other dependencies are installed
2. Build the project using your preferred method
3. Run the application
4. Navigate to "Parts" and try searching

## Dependencies

- SFML 3.x
- nlohmann/json
- curl
- C++17 compiler

## Note

Without setting up the API credentials, the search functionality will show an error message, but the rest of the application will work normally.
