import { Component } from '@angular/core';

@Component({
  standalone: true,
  selector: 'app-main',
  imports: [],
  templateUrl: './main.html',
  styleUrl: './main.css',
})
export class Main {
  isCircleHovered = false;
  isCardHovered = false;
  selectedFile: File | null = null;

  onCircleHover() {
    this.isCircleHovered = true;
  }

  onCircleLeave() {
    this.isCircleHovered = false;
  }

  onCardHover() {
    this.isCardHovered = true;
  }

  onCardLeave() {
    this.isCardHovered = false;
  }

  onFileSelected(event: any) {
    const file: File = event.target.files[0];
    if (file) {
      this.selectedFile = file;
      console.log('Selected file:', file.name, file.type, file.size);
    }
  }

  uploadFile() {
    if (this.selectedFile) {
      // Call your audio service here
      console.log('Uploading file:', this.selectedFile.name);
      this.selectedFile = null;
    }
  }
}
