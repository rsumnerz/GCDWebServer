/*
  Copyright (c) 2012-2013, Pierre-Olivier Latour
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
  * Redistributions of source code must retain the above copyright
  notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright
  notice, this list of conditions and the following disclaimer in the
  documentation and/or other materials provided with the distribution.
  * Neither the name of the <organization> nor the
  names of its contributors may be used to endorse or promote products
  derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#import "GCDWebServerRequest.h"
#import "GCDWebServerResponse.h"

typedef GCDWebServerRequest* (^GCDWebServerMatchBlock)(NSString* requestMethod, NSURL* requestURL, NSDictionary* requestHeaders, NSString* urlPath, NSDictionary* urlQuery);
typedef GCDWebServerResponse* (^GCDWebServerProcessBlock)(GCDWebServerRequest* request);

@class GCDWebServer, GCDWebServerHandler;

@interface GCDWebServerConnection : NSObject {
@private
  GCDWebServer* _server;
  NSData* _address;
  CFSocketNativeHandle _socket;
  NSUInteger _bytesRead;
  NSUInteger _bytesWritten;
  
  CFHTTPMessageRef _requestMessage;
  GCDWebServerRequest* _request;
  GCDWebServerHandler* _handler;
  CFHTTPMessageRef _responseMessage;
  GCDWebServerResponse* _response;
}
@property(nonatomic, readonly) GCDWebServer* server;
@property(nonatomic, readonly) NSData* address;  // struct sockaddr
@property(nonatomic, readonly) NSUInteger totalBytesRead;
@property(nonatomic, readonly) NSUInteger totalBytesWritten;
@end

@interface GCDWebServerConnection (Subclassing)
- (void) open;
- (GCDWebServerResponse*) processRequest:(GCDWebServerRequest*)request withBlock:(GCDWebServerProcessBlock)block;
- (void) close;
@end

@interface GCDWebServer : NSObject {
@private
  NSMutableArray* _handlers;
  
  NSUInteger _port;
  NSRunLoop* _runLoop;
  CFSocketRef _socket;
  CFNetServiceRef _service;
}
@property(nonatomic, readonly, getter=isRunning) BOOL running;
@property(nonatomic, readonly) NSUInteger port;
- (void) addHandlerWithMatchBlock:(GCDWebServerMatchBlock)matchBlock processBlock:(GCDWebServerProcessBlock)processBlock;
- (void) removeAllHandlers;

- (BOOL) start;  // Default is main runloop, 8080 port and computer name
- (BOOL) startWithRunloop:(NSRunLoop*)runloop port:(NSUInteger)port bonjourName:(NSString*)name;  // Pass nil name to disable Bonjour or empty string to use computer name
- (void) stop;
@end

@interface GCDWebServer (Subclassing)
+ (Class) connectionClass;
+ (NSString*) serverName;  // Default is class name
@end

@interface GCDWebServer (Extensions)
- (BOOL) runWithPort:(NSUInteger)port;  // Starts then automatically stops on SIGINT i.e. Ctrl-C (use on main thread only)
@end

@interface GCDWebServer (Handlers)
- (void) addDefaultHandlerForMethod:(NSString*)method requestClass:(Class)class processBlock:(GCDWebServerProcessBlock)block;
- (void) addHandlerForBasePath:(NSString*)basePath localPath:(NSString*)localPath indexFilename:(NSString*)indexFilename cacheAge:(NSUInteger)cacheAge;  // Base path is recursive and case-sensitive
- (void) addHandlerForMethod:(NSString*)method path:(NSString*)path requestClass:(Class)class processBlock:(GCDWebServerProcessBlock)block;  // Path is case-insensitive
- (void) addHandlerForMethod:(NSString*)method pathRegex:(NSString*)regex requestClass:(Class)class processBlock:(GCDWebServerProcessBlock)block;  // Regular expression is case-insensitive
@end
