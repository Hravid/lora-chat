'use client'
import React, { useEffect, useState } from 'react';
import PocketBase from 'pocketbase';


// Use environment variable instead of hardcoded URL
const pb = new PocketBase(process.env.NEXT_PUBLIC_POCKETBASE_URL)

const MyPage = () => {
  const [receivedMessage, setReceivedMessage] = useState<any>(null);
  const [sentMessage, setSentMessage] = useState<string>(""); // Initialize with empty string
  const [isLoading, setIsLoading] = useState<boolean>(false);
  
  const fetchMessage = async () => {
    try {
      setIsLoading(true);
      const response = await fetch(`${process.env.NEXT_PUBLIC_MESSAGE_API_URL}/get-message`);
      const result = await response.json();
      setReceivedMessage(result);
    } catch (error) {
      console.error("Error fetching data:", error);
    } finally {
      setIsLoading(false);
    }
  };

  const postMessage = async () => {
    if (!sentMessage.trim()) {
      alert("Please enter a message");
      return;
    }
    
    try {
      setIsLoading(true);
      const response = await fetch(`${process.env.NEXT_PUBLIC_MESSAGE_API_URL}/post-message`, {
        method: "POST",
        headers: {
          "Content-Type": "text/plain",
        },
        body: sentMessage,
      });
      
      // Get text first since ESP32 might not return valid JSON
      
      
      setSentMessage(""); // Clear input after sending
    } catch (error) {
      console.error("Error sending data:", error);
    } finally {
      setIsLoading(false);
    }
  };

  // Parse data for display
  function dataParse(data: any): string {
    try {
      if (data && typeof data === 'object') {
        return data.message || JSON.stringify(data);
      } else if (typeof data === 'string') {
        try {
          const parsed = JSON.parse(data);
          return parsed.message || JSON.stringify(parsed);
        } catch (e) {
          return data;
        }
      }
      return String(data);
    } catch (error) {
      console.error("Error parsing data:", error);
      return "Error parsing data";
    }
  }

  useEffect(() => {
    fetchMessage();
  }, []);

  return (
    <div className="p-4 max-w-lg mx-auto mt-8">
      <h1 className="text-2xl font-bold mb-4">LoRa chat</h1>
      
      <div className="mb-6 bg-gray-100 p-4 rounded">
        <label className="block text-gray-700 mb-2">Message to send:</label>
        <div className="flex">
          <input
            type="text"
            value={sentMessage}
            onChange={(e) => setSentMessage(e.target.value)}
            className="border p-2 flex-grow"
            placeholder="Type your message here"
          />
          <button 
            onClick={postMessage}
            disabled={isLoading}
            className="bg-blue-500 text-white p-2 ml-2 rounded hover:bg-blue-600 disabled:bg-gray-400">
            Send
          </button>
        </div>
      </div>
      
      <div className="mb-6">
        <div className="flex justify-between items-center mb-2">
          <h2 className="text-xl font-bold">Response:</h2>
          <button 
            onClick={fetchMessage}
            disabled={isLoading}
            className="bg-green-500 text-white p-2 rounded hover:bg-green-600 disabled:bg-gray-400 text-sm">
            Refresh
          </button>
        </div>
        
        <div className="bg-gray-100 p-4 rounded min-h-[100px]">
          {isLoading ? (
            <p className="text-gray-500">Loading...</p>
          ) : receivedMessage ? (
            <pre className="whitespace-pre-wrap">{dataParse(receivedMessage)}</pre>
          ) : (
            <p className="text-gray-500">No message received yet</p>
          )}
        </div>
      </div>
    </div>
  );
};

export default function Page() {
  return (
      <MyPage />
  );
}