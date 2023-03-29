import { Component } from '@angular/core';
import { MdbModalRef } from 'mdb-angular-ui-kit/modal';
import { ParameterService } from '../services/parameter.service';
import { Parameters } from '../utils/interfaces';

@Component({
  selector: 'app-type-modal',
  templateUrl: './type-modal.component.html',
  styleUrls: ['./type-modal.component.css'],
})
export class TypeModalComponent {
  name: string = '';
  type: string = '';

  constructor(
    private modalRef: MdbModalRef<TypeModalComponent>,
    private parameterService: ParameterService
  ) {}

  close() {
    this.modalRef.close();
  }

  submit() {
    this.parameterService.newParamSet(
      new Parameters({ name: this.name, type: this.type })
    );
    this.close();
  }

  validInputs(): boolean {
    return !!this.type && !!this.name;
  }

  onTypeChanged(event: Event) {
    this.type = (event.target as HTMLInputElement).value;
  }

  onNameChanged(event: Event) {
    this.name = (event.target as HTMLInputElement).value;
  }
}
