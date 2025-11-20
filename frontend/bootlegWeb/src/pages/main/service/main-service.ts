import { inject, Injectable } from '@angular/core';
import { HttpClient } from '@angular/common/http';
@Injectable({
  providedIn: 'root',
})
export class MainService {
  private http = inject(HttpClient);

  private apiUrl = 'http://localhost:8080/api';

  recogniseSong(audioFile: File) {
    console.log('recognising song');
    const formData = new FormData();
    formData.append('audio', audioFile);
    this.http.post(`${this.apiUrl}/recognise`, formData).subscribe((res) => {});
  }
  addSong(audioFile: File) {
    console.log('adding song');
    const formData = new FormData();
    formData.append('audio', audioFile);
    this.http.post(`${this.apiUrl}/addSong`, formData).subscribe((res) => {});
  }
}
