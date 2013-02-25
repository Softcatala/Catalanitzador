/*
 * Copyright (C) 2011 Jordi Mas i Hernàndez <jmas@softcatala.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 */

#include "HttpFormInet.h"

// Encodes a single variable form
void HttpFormInet::UrlFormEncode(vector <string> variables, vector <string> values, string& encoded)
{
    assert(variables.size() == values.size());
    
    encoded.erase();
    
    for (unsigned int v = 0; v < variables.size(); v++)
    {
        if (v > 0)
            encoded+= "&";
        
        encoded+= variables.at(v);
        encoded+= "=";
        
        unsigned char* pos = (unsigned char *) values.at(v).c_str();
        for (unsigned int i = 0; i < values.at(v).size(); i++, pos++)
        {
            if (*pos =='.' ||
                (*pos >='0' && *pos <='9') ||
                (*pos >='A' && *pos <='Z') ||
                (*pos >='a' && *pos <='z') ||
                (*pos == '\r' || *pos == '\n' || *pos == '\t'))
            {
                encoded += *pos;
            }
            else if (*pos == ' ')
            {
                encoded += '+';
            }
            else
            {
                char string[16];
                sprintf(string, "%%%X", *pos);
                encoded += string;
            }
        }
    }
}


// Variables are added in ANSI at the end of the request that's why is an ANSI string
// See: http://stackoverflow.com/questions/9065583/cocoa-http-post-request-mac-os-x
bool HttpFormInet::PostForm(string _url, vector <string> variables, vector <string> values)
{
    bool sent;
    string encoded;
    UrlFormEncode(variables, values, encoded);
    
    NSLog(@"web request started");
    NSString *post = [NSString stringWithUTF8String:encoded.c_str()];
    NSData *postData = [post dataUsingEncoding:NSUTF8StringEncoding];
    NSString *postLength = [NSString stringWithFormat:@"%ld", (unsigned long)[postData length]];
    
    NSLog(@"Post data: %@", post);
    
    NSMutableURLRequest *request = [NSMutableURLRequest new];
    NSString* url = [NSString stringWithUTF8String:_url.c_str()];
    [request setURL:[NSURL URLWithString:url]];
    [request setHTTPMethod:@"POST"];
    [request setValue:postLength forHTTPHeaderField:@"Content-Length"];
    [request setValue:@"application/x-www-form-urlencoded" forHTTPHeaderField:@"Content-Type"];
    [request setHTTPBody:postData];
    
    NSURLResponse* response;
    NSError* error = nil;
    [NSURLConnection sendSynchronousRequest:request  returningResponse:&response error:&error];
    
    sent = (error == NULL);
    if (error != NULL)
    {
        NSLog(@"HttpFormInet::PostForm. Error: %@", error);
    }
    
    NSLog(@"HttpFormInet::PostForm. Sent: %u", sent);
    return sent;
}
