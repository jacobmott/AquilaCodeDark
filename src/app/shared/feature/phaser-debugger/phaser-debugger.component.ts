import {
  Component,
  ViewEncapsulation,
  ViewChild,
  ElementRef,
  OnInit,
  AfterViewChecked,
} from '@angular/core';
import { CommonModule } from '@angular/common';
import { ReactiveFormsModule } from '@angular/forms';
import { SharedService } from '../../data-access/shared.service';
import { ScrollData } from '../../util/shared-types';

@Component({
  selector: 'app-aquila-phaser-debugger-test',
  standalone: true,
  imports: [CommonModule, ReactiveFormsModule],
  templateUrl: './phaser-debugger.component.html',
  styleUrl: './phaser-debugger.component.css',
  encapsulation: ViewEncapsulation.Emulated,
})
export class PhaserDebuggerComponent implements OnInit, AfterViewChecked {
  @ViewChild('scrollMe') private myScrollContainer: ElementRef;

  arrowFillColor = '#8E37E6';
  mainClass =
    'z-11 bg-aquila-400 -bottom-2/6 fixed left-1 h-2/6 w-3/6  rounded-md border-8 transition-all duration-300 ease-in hover:bottom-1';
  mainClassToggle =
    'z-11 bg-aquila-400 -bottom-2/6 fixed left-1 h-2/6 w-3/6  rounded-md border-8 transition-all duration-300 ease-in hover:bottom-1';
  toggle = false;
  wantScrollBottomBehavior = true;

  scrolledData: ScrollData[] = [];
  dataPoint: any;

  constructor(private sharedService: SharedService) {
    // this.myForm = new FormGroup({
    //   message: new FormControl(),
    // });
    this.scrolledData = this.sharedService.scrolledData;
    this.dataPoint = this.sharedService.dataPoint;
  }

  ngAfterViewChecked(): void {
    this.scrollToBottom();
  }

  onScroll(event: any): void {
    this.checkWantScrollBottomBehavior();
  }

  isAtBottom(): boolean {
    const sh = Math.trunc(this.myScrollContainer.nativeElement.scrollHeight);
    const st = Math.trunc(this.myScrollContainer.nativeElement.scrollTop);
    const ch = Math.trunc(this.myScrollContainer.nativeElement.clientHeight);
    const value1 = Math.trunc(sh - st);
    const value2 = Math.trunc(ch);
    // console.log("sh: " + sh);
    // console.log("st: " + st);
    // console.log("ch: " + ch);
    // console.log("sh-st = " + value1);
    // console.log("ch = " + value2);
    //just need to be within +-1
    return value1 === value2 || value1 === value2 + 1 || value1 === value2 - 1;
  }

  checkWantScrollBottomBehavior() {
    if (this.isAtBottom()) {
      // Do something when the scrollbar is  at the bottom
      // console.log("At the bottom");
      this.wantScrollBottomBehavior = true;
    } else {
      // console.log("Not at the bottom");
      this.wantScrollBottomBehavior = false;
    }
  }

  scrollToBottom(): void {
    if (!this.wantScrollBottomBehavior) {
      return;
    }
    this.myScrollContainer.nativeElement.scrollTop =
      this.myScrollContainer.nativeElement.scrollHeight;
  }

  ngOnInit() {}

  onPointerDown(event: MouseEvent) {
    if (this.toggle) {
      this.toggle = false;
      this.mainClass = this.mainClassToggle;
    } else {
      this.mainClass =
        'z-11 bg-aquila-400 bottom-1 fixed left-1 h-2/6 w-3/6 rounded-md border-8';
      this.toggle = true;
    }
  }

  getClasses() {
    return this.mainClass;
  }
}
