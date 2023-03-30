import {
  AfterViewInit,
  Component,
  ElementRef,
  Input,
  OnChanges,
  OnInit,
  QueryList,
  SimpleChanges,
  ViewChild,
  ViewChildren,
} from '@angular/core';
import { ParameterService } from '../services/parameter.service';
import { Callback, CodeType, Parameters } from '../utils/interfaces';
import { UUID } from '../utils/utils';

@Component({
  selector: 'app-parameter',
  templateUrl: './parameter.component.html',
  styleUrls: ['./parameter.component.css'],
})
export class ParameterComponent implements OnInit, AfterViewInit, OnChanges {
  @ViewChild('codeDiv', { static: true })
  codeDiv?: ElementRef<HTMLDivElement>;
  @ViewChild('codeDisplay', { static: true })
  codeDisplay?: ElementRef<HTMLDivElement>;
  @ViewChild('codeInput', { static: true })
  codeInput?: ElementRef<HTMLTextAreaElement>;
  @ViewChild('lineNums', { static: true })
  lineNums?: ElementRef<HTMLDivElement>;
  @ViewChildren('codeLine') codeLines?: QueryList<ElementRef<HTMLDivElement>>;

  @Input() callback: Callback = new Callback();
  parameters: { set: Parameters; params: Set<string> }[] = [];
  signature: [string, CodeType][] = [];
  code: string = '';
  name: string = '';
  idxs: number[][] = [[0, 0]];
  x_offs: number[][] = [];

  CodeType = CodeType;

  constructor(protected parameterService: ParameterService) {}

  ngOnInit() {}

  ngAfterViewInit(): void {
    this.codeLines?.changes.subscribe(() => {
      this.updateText();
    });
    this.updateText();
  }

  ngOnChanges(changes: SimpleChanges) {
    this.name = this.callback.name;
    if (!this.callback.code.endsWith('\n')) {
      this.callback.code += '\n';
    }
    this.code = this.callback.code;
    this.parameters = Object.entries(this.callback.params).map(
      ([uuid, params]: [string, Set<string>]) => ({
        set: this.parameterService.getParamSet(uuid),
        params: params,
      })
    );
    this.signature = this.callback.getSignatureSplit(
      this.parameterService.paramSetDict
    );
    setTimeout(() => {
      if (this.codeInput && this.codeDisplay && this.codeDiv) {
        this.codeInput.nativeElement.style.height = `${this.codeDisplay.nativeElement.offsetHeight}px`;
        this.codeInput.nativeElement.scrollTo(0, 0);
        this.codeDiv.nativeElement.scrollTo(0, 0);
      }
    }, 0);
  }

  update() {
    if (!this.name || !this.code) {
      return;
    }

    this.callback.code = this.code;
  }

  updateText() {
    if (this.codeLines) {
      let x_offs: number[][] = [...Array(this.codeLines.length)].map(() => []);
      this.codeLines.forEach((el: ElementRef<HTMLDivElement>) => {
        let div: HTMLDivElement = el.nativeElement;
        let id: number = +div.id;
        let x: number = 0;
        Object.values(div.children).forEach((child: Element) => {
          x_offs[id].push(x);
          x += child.clientWidth;
        });
      });
      if (
        x_offs.findIndex((row: number[], r: number) => {
          return (
            this.x_offs[r] === undefined ||
            row.findIndex((v: Number, c: number) => v !== this.x_offs[r][c]) !==
              -1
          );
        }) !== -1
      ) {
        setTimeout(() => {
          this.x_offs = x_offs;
        }, 0);
      }
    }
  }

  onCodeChanged(event: Event) {
    this.code = (event.target as HTMLTextAreaElement).value;
  }

  onNameChanged(event: Event) {
    this.name = (event.target as HTMLInputElement).value;
  }

  onScroll(event: Event) {
    let scrollX: number = (event.target as HTMLTextAreaElement).scrollLeft;
    let scrollY: number = (event.target as HTMLTextAreaElement).scrollTop;
    if (this.codeDisplay) {
      this.codeDisplay.nativeElement.scrollTo(scrollX, scrollY);
    }
    if (this.lineNums) {
      this.lineNums.nativeElement.scrollTo(scrollX, scrollY);
    }
  }

  onCodeKeyPress(event: KeyboardEvent) {
    if (event.ctrlKey && event.key === 's') {
      this.save();
      event.preventDefault();
    }
  }

  save() {
    if (this.name) {
      this.callback.name = this.name;
    }
    if (!this.code.endsWith('\n')) {
      this.code += '\n';
    }
    this.callback.code = this.code;
  }

  select(pid: UUID, name: string) {
    console.log('Show Parameters:', pid, name);
  }

  isNameUsed(): boolean {
    return false;
  }

  getVarIdxs(): number[][] {
    let regex: string = Object.values(this.callback.params)
      .map((params: Set<string>) => [...params].join('|'))
      .join('|');
    let global_idx: number = 0;
    return this.code.split('\n').map((l: string) => {
      if (!l) {
        global_idx += 1;
        return [];
      }
      let res: RegExpMatchArray | null = l.match(regex);
      let idxs: number[] = [global_idx];
      while (res && res.index !== undefined) {
        let idx: number = res.index;
        idxs.push(global_idx + idx);
        idx += res[0].length;
        idxs.push(global_idx + idx);
        global_idx += idx;
        l = l.slice(idx);
        res = l.match(regex);
      }
      global_idx += l.length;
      idxs.push(global_idx);
      global_idx += 1;
      return idxs;
    });
  }
}
