import { Component, inject } from '@angular/core';
import { MainService } from './service/main-service';
import { CommonModule } from '@angular/common';

@Component({
  standalone: true,
  selector: 'app-main',
  imports: [CommonModule],
  templateUrl: './mainPage.html',
  styleUrl: './mainPage.css',
})
export class MainPage {
  private mainService = inject(MainService);

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
      console.log('Uploading file:', this.selectedFile.name);
      this.mainService.recogniseSong(this.selectedFile);
      this.selectedFile = null;
    }
  }
}
